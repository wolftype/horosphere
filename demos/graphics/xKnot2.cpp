/*
 * =====================================================================================
 *
 *       Filename:  xKnot2.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2015 19:56:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */



#include "horo_control.h" 
#include "horo_render.h"

#include "horo_vsr.hpp"

#include "vsr/form/vsr_knot.h"
#include "vsr/form/vsr_shapes.h"

//#include "data/xKnot.h"

using namespace al;
using namespace vsr;
using namespace vsr::cga;


/*-----------------------------------------------------------------------------
 *  SHARED DATA
 *-----------------------------------------------------------------------------*/
struct State{
  float time;
  gfx::Pose model;
  gfx::Pose camera;
  Point point;

  float P, Q, theta, phi, vel, rotvel, tube_size, writhe;

  float energy, energy_scale; 
  
  float size;

  int numcables;
                 
  float diameter;
  
  bool bAutoMode;
  bool bDrawRibbon, bDrawFibers, bDrawVec, bDrawTube, bDrawPnt;     
  bool bDrawWrithe, bUseEnergies, bFlow; 
  
  vsr::cga::Vec vec;                               //<-- Hopf Vec
  Point pnt = Construct::point(3,0,0);  	   //<-- Point Along Orbit

  float ecc;                                       //<-- hyperbolicness of orbit (wt of points)

};


/*-----------------------------------------------------------------------------
 *  LOCAL DATA
 *-----------------------------------------------------------------------------*/
struct Local{
  
  TorusKnot tk;
  
  //MESHES
  gfx::Mesh tm;             // Main Torus Knot Mesh   
  gfx::Mesh tube;           // Tube Mesh   

  vector< gfx::Mesh > wm;   // Some number of writhe Meshes;

  
  /*-----------------------------------------------------------------------------
   *  Update State on Control App
   *-----------------------------------------------------------------------------*/
  void updateState(State * state){
    State& s = *state;
   
    if(s.bFlow) s.pnt = Round::loc( s.pnt.spin( tk.bst() ) );

    vsr::cga::Vec tvec;
   
    if (s.bAutoMode){
     tvec = vsr::cga::Vec(s.pnt).unit();
     if (s.bUseEnergies){
       s.writhe = s.energy / s.energy_scale;
     }
    } else {
     tvec =  vsr::cga::Vec::x.spin( Gen::rot(s.theta, s.phi) );
    }

    Rot r = Gen::rot( Gen::log( Gen::ratio(tvec, s.vec) ) * s.rotvel );  
    s.vec = s.vec.spin( r );
    
  }

  
  /*-----------------------------------------------------------------------------
   *  Update Mesh Data on All Apps
   *-----------------------------------------------------------------------------*/
  void updateMeshData(State * state){
    State& s = *state;
    
    //Knot Settings
    tk.HF.vec() = s.vec;
    tk.P = s.P;
    tk.Q = s.Q;
    tk.amt = s.vel;
    
    //Knot Calculation
    tk.calc0( s.pnt); //Round::null(3,0,0) );
    auto energy = tk.energy(0,tk.pnt.size());

    //Mesh Data
    tm.clear();
    wm.clear();
    tube.clear();

    // Tube Mesh
    int res = 5;
    tube = Shape::Skin( tk.cir, tk.cir.size(), res );
  
    // Tube Color by Energy 
    for (int i = 0; i<tk.cir.size();++i){
      double c = tk.energies[i]* s.energy_scale; 
      for (int j=0; j<res;++j){  
        tube[i + j * tk.cir.size() ].Col.set(c, 0, 1-c, .5); 
      }
    }


   //ADD POINTS TO MESH COLORED BY ENERGY
//    for (int i = 0; i<tk.pnt.size(); ++i){
//     double c = tk.energies[i]* s.energy_scale;
//     tm.add( Vertex(tk.pnt[i], gfx::Vec3f(0,0,1), gfx::Vec4f( 1 , 1-c, .5, 1 ) ) );  
//    }
    

//    // WRITHE CABLES  ...
//   for (int k = 0; k < s.numcables/2.; ++k){
//       
//       wm.push_back( gfx::Mesh(GL::LL) );
//       wm.push_back( gfx::Mesh(GL::LL) );  
// 
//       for(int i = 0; i < iter; ++i){
//     
//         double c = tk.energies[i]* s.energy_scale; 
//         double t = 1.0 * i/iter;
//       
//         if (s.bUseEnergies) t *= tk.energies[i] * s.energy_scale;
//    
//         Par tp = Round::par_cir( tk.cir[i], t * TWOPI * s.writhe);  
//         Pnt pa = Round::split( tp, true );
//         Pnt pb = Round::split( tp, false );
// 
//         wm[k].add(  pa );
//         wm[k+1].add( pb );
// 
//      }
//   }
   
  }

  void draw(State * state){
    State& s = *state;
    
    glPointSize(10);  
 
//    tm.drawVertices();  
    
    if (s.bDrawFibers) {
      Draw( tk.HF.fiberA(), 0,1,0 ); 
      Draw( tk.HF.fiberB(), 1,0,1 ); 
    }
    
    if (s.bDrawVec) Draw(s.vec, 1, 1, 0 ); 
  
    if (s.bDrawPnt) Draw(s.pnt,1,0,0);  
//      
//    if (s.bDrawWrithe) {
//      for(auto& i : wm ){
//        i.drawVertices();
//      }   
//    }
  
    if (s.bDrawTube) tube.drawElementsColor(); 
    
  }

};



/*-----------------------------------------------------------------------------
 *  CONTROL BONE
 *-----------------------------------------------------------------------------*/
struct ControlApp : ControlBone<State> {
 


  //Mouse Position
  Point mouse;

  //Local Data
  Local local;

  //Some Variables
  bool bReset = false;
  float amt = 0;

  // Construct with broadcast ip
  ControlApp(const char * ip) : ControlBone<State>(ip) {}


  /*-----------------------------------------------------------------------------
   *  Setup Variables
   *-----------------------------------------------------------------------------*/
  void setup(){
    State& s = *(this->state);
    ///Bind Gui
    bindGLV();
    ///Add Variables to GUI
    gui( s.P, "P", 0, 100)( s.Q, "Q", 0, 100); 
    gui( s.vel, "vel",0.001,100);
    gui( s.tube_size, "tube_size", 0, 10);
    gui( s.writhe, "writhe", 1,100); 
    gui( s.theta, "theta", 0, 3.14) ( s.phi, "phi", -3.14/4.0, 3.14/4.0 );  
    gui( s.rotvel, "rotvel", -10,10);
    
    gui( s.energy_scale, "energy_scale",0,1000); 
    gui( s.bUseEnergies, "use_energies");
    
    gui( s.bFlow, "flow"); 
    gui( s.bAutoMode, "mode"); 
    gui( s.bDrawWrithe, "draw_writhe"); 
    gui( s.bDrawRibbon, "draw_ribbon");
    gui( s.bDrawFibers, "draw_fibers");
    gui( s.bDrawVec, "draw_vec");
    gui( s.bDrawTube, "draw_tube"); 
    gui( s.bDrawPnt, "draw_pnt"); 
    gui( s.size, "size", .1, 100); 

    gui.arrange();
  
    //Defaults
    s.P = 3; s.Q = 2;
    s.vel = .015;
    s.writhe = 10;  
    s.numcables = 2;
    s.tube_size = 1.;  
    s.rotvel = .01;
  
    s.bDrawWrithe = true;
    s.bFlow = true;
  
    s.energy_scale = 20; 

    s.vec = vsr::cga::Vec(0,1,0);
    s.pnt = Round::null(3,0,0);

    
  }

  void onAnimate(){
    auto &s = *(this->state);
    s.time += .01;

    state -> point = mouse;
    state -> camera = (gfx::Pose)scene.camera;
    state -> model = scene.model;

    local.updateState(state);
    local.updateMeshData(state);

  }

  /*-----------------------------------------------------------------------------
   *  Draw Routines 
   *-----------------------------------------------------------------------------*/
  void onDraw(){
    mouse = calcMouse3D();  

    local.draw(state);
    //Draw(CXY(1));

  }
  
};




/*-----------------------------------------------------------------------------
 *  RENDER BONE
 *-----------------------------------------------------------------------------*/
struct RenderApp : OmniRenderBone<State>{

   Local local;
  /*-----------------------------------------------------------------------------
   *  Draw Loop Called Multiple Times Per Frame
   *-----------------------------------------------------------------------------*/
   void onDraw(Graphics& g){
     
     //MODEL pose to axis angle
     glPushMatrix();
       Vec4<> tr = state->model.quat().axan(); 
       glRotatef ( tr[3], tr[0], tr[1], tr[2]  );  
      
       local.draw(state);   
            
    glPopMatrix();
   }

   /*-----------------------------------------------------------------------------
    *  Physics Called Once per Frame (do something with state)
    *-----------------------------------------------------------------------------*/
   void onAnimate(al_sec dt){
      auto& s = *(this->state);
      //cout << s.time << endl;
      pose = gfx2al::pose( state->camera ); //conversion to al world 

      local.updateMeshData(state);

   }

};  



/*-----------------------------------------------------------------------------
 *  Call Executable with ANY argument to launch control 
 *-----------------------------------------------------------------------------*/
int main (int argc, char *argv[]){

   if (argc > 1) {
     if (argv[1] == "allo"){
      printf("AlloDemo\n");
      ControlApp app("192.168.10.255");
      app.start();
     } else {
      printf("%s \t", argv[1] );
      printf("LocalDemo\n");
       ControlApp app("127.0.0.1");
       app.start();
     }
   } else {
     RenderApp app;
     app.start();
   }

    return 0;
}

