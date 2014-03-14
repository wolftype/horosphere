/*
 * =====================================================================================
 *
 *       Filename:  xKnot.cpp
 *
 *    Description:  knot 2.0
 *
 *        Version:  1.0
 *        Created:  02/25/2014 13:38:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#include "al_OmniAppGFX.h"   
#include "gam_GxSync.h"

#include "vsr/vsr_cga3D_op.h"
#include "vsr/vsr_cga3D_draw.h"
#include "vsr/vsr_knot.h"

//GRAPHICS
#include "gfx/gfx_mesh.h"
#include "vsr/vsr_shapes.h"

//Data Containers
#include "data/vsr_knotData.h"
#include "data/vsr_audioData.h"


#include "al_glv_gui.h"

using namespace al;
using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;

struct App : OmniApp {
    
    Light light;

    //DATA STORAGE OBJECTS to move around network (floats)
    KnotData kd;
    AudioData ad;

    //A Knot
    TorusKnot tk; 
    
    Pnt pnt; // <-- point along knot
    vsr::Vec vec; // <-- Vector of hopf fiber
     
    //MESHES
    gfx::Mesh tm;             // Main Torus Knot Mesh   
    gfx::Mesh tube;           // Tube Mesh     
    
    vector< vsr::Mesh > wm;   // Some number of writhe Meshes;
    
    //bSlave flag = true if machine is "receiver" machine
    App(bool bSlave = false) :  
      OmniApp("vortex",bSlave), 
      tk(3,2,.01),
      kd(),
      pnt(Ro::null(2,0,0)),
      vec(0,1,0)
    {
          
              
      init();
      
    }     

    virtual ~App() {}

    void init();
    void step();
  
    virtual void onAnimate(al_sec dt);
    virtual void onDraw(Graphics& g);

    virtual void updateState();
    virtual void sendAudioData(); 
     
    virtual void onMessage(osc::Message& m);    

};

/*-----------------------------------------------------------------------------
 *  INITIALIZATION
 *-----------------------------------------------------------------------------*/
void App::init(){
  tm.mode(GL::LL); 
  kd.buildGui(glv.gui); 
}


/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void App::onAnimate(al_sec dt){
        
    if (!bSlave){     
      sendAudioData(); 
    } 
  
    step();      
}


/*-----------------------------------------------------------------------------
 * UPDATE STATE (MASTER) //Only the MASTER computer does this
 *-----------------------------------------------------------------------------*/
void App::updateState(){
  
  //Calculate Point Along Orbit
  //pnt = Ro::null( kd.pntX, kd.pntY, kd.pntZ);
  pnt.print();

  if (kd.bFlow)  pnt = Ro::loc( pnt.sp( tk.bst() ) );
  
  kd.pntX = pnt[0];
  kd.pntY = pnt[1];
  kd.pntZ = pnt[2];
  
  //Calculate Hopf Fiber Orientation    
  vsr::Vec tvec;
  if (kd.bAutoMode){  
    tvec = vsr::Vec(pnt).unit();       
    if (kd.bUseEnergies) kd.writhe = kd.energy / kd.energy_scale;     
  } else {
    tvec =  vsr::Vec::x.sp( Gen::rot(kd.theta, kd.phi) );
  }
  
  Biv tb = Gen::log( Gen::ratio(tvec, vec) ); 
  Rot r = Gen::rot( tb * kd.rotVel );  
  vec = vec.sp( r );

  kd.vecX = vec[0];
  kd.vecY = vec[1];
  kd.vecZ = vec[2];

  //Send Data
  kd.bundleAndSend();

}

/*-----------------------------------------------------------------------------
 *  Send Audio Data To Audio App
 *-----------------------------------------------------------------------------*/
void App::sendAudioData(){ 

      //Package and Send Audio Info
      ad.freq = 330. + ( 110. * ( 10 * kd.diameter ) ); 
      ad.width = 10 + ( 8000. * kd.diameter );  
      ad.nrg = kd.energy / 100.;     /// Hovers around 1
      ad.ftheta =  vsr::Vec(Op::pj( vec, Biv::xz)).norm();  
      ad.fphi =  vsr::Vec(Op::pj( vec, Biv::xy)).norm();  
      ad.fwrithe = kd.writhe/6.0;    
      ad.ratio = .01 * kd.P / kd.Q;
      ad.nftheta = 40 + std::fabs(kd.theta) * 40; 
      ad.nfphi = 40 + std::fabs(kd.phi) * 40; 
    
      ad.bundleAndSend();  
  
}





void App :: onMessage(osc::Message& m) {   
    if (bSlave){  
      OmniApp::onMessage(m);
      kd.onMessage(m);
      //cout <<"receiving message" << endl;
    }  
}



/*-----------------------------------------------------------------------------
 *  BUILD MESHES
 *-----------------------------------------------------------------------------*/
void App::step(){

  if (!bSlave ) updateState();
 
  light();

  // Clear State
  tk.cir.clear();
  tk.pnt.clear();
    
  // CLEAR MESHES
  tm.clear();   
  wm.clear();
  tube.clear(); 

  // SET KNOT PARAMETERS
  vec = vsr::Vec(kd.vecX, kd.vecY, kd.vecZ);
  //pnt = Ro::null( kd.pntX, kd.pntY, kd.pntZ);

  //pnt.print();

  tk.HF.vec() = vec; 
  tk.P = kd.P;
  tk.Q = kd.Q;
  tk.amt = kd.vel; 
  int iter = tk.iter(); 

  // LOCAL STATICS
  static Pnt np;           // New Position  
  np = pnt;   
  
  static Bst bst;          // Boostor
  bst = tk.bst();

  // ORBIT POINTS
  for (int i = 0; i<iter+1;++i){
      np = Ro::loc( np.sp( bst ) );
      tk.pnt.push_back(np);
  }

  //SET Knot Energy
  kd.energy = tk.energy(0, iter+1);  
  
  //ADD POINTS TO MESH COLORED BY ENERGY
  for (int i = 0; i<iter+1;++i){
    double c = tk.energies[i]* kd.energy_scale;
    tm.add( Vertex(tk.pnt[i], gfx::Vec3f(0,0,1), gfx::Vec4f( 1 , 1-c, .5, 1 ) ) );  
  }
  
//     cout << "ITER**** " << iter << endl;  
  // TUBE CIRCLES
  for (int i = 0; i<iter;++i){
     int nextIdx = i < iter ? i + 1 : 0;
     Par tpar = tk.pnt[i] ^ tk.pnt[nextIdx];
     tk.cir.push_back( tpar.dual().dil( tk.pnt[i], kd.tube_size) );
  }   
    
  kd.diameter = Ro::size( tk.cir[0], true );
    
  // Tube Mesh
  tube = Shape::Skin( tk.cir, tk.cir.size(), 10 );
   
  for (int i = 0; i<iter;++i){
  
    double c = tk.energies[i]* kd.energy_scale; 
  
    for (int j=0; j<10;++j){  
     tube[i + j * iter ].Col.set(c, 0, 1-c, .5); 
    }
  }

   // WRITHE CABLES  ...
  for (int k = 0; k < kd.numcables/2.; ++k){
      
      wm.push_back( gfx::Mesh(GL::LL) );
      wm.push_back( gfx::Mesh(GL::LL) );  

      for(int i = 0; i < iter; ++i){
    
      double c = tk.energies[i]* kd.energy_scale; 
      double t = 1.0 * i/iter;
    
      if (kd.bUseEnergies) t *= tk.energies[i] * kd.energy_scale;
 
      Par tp = Ro::par_cir( tk.cir[i], t * TWOPI * kd.writhe);  
      Pnt pa = Ro::split( tp, true );
      Pnt pb = Ro::split( tp, false );

      wm[k].add(  pa );
      wm[k+1].add( pb );//Vertex(pb, 1-c, 0, c );
        //ribbon.add( pa); ribbon.cp.push_back( Vec(1-c, 0, c ) );
        //ribbon.add( pb); ribbon.cp.push_back( Vec(1-c, 0, c ) );  
     }
  }

}



/*-----------------------------------------------------------------------------
 *  CALLED MANY TIMES PER FRAME
 *-----------------------------------------------------------------------------*/
void App::onDraw(Graphics& g) {

  glPointSize(10);  
   
  tm.drawVertices(0,1,0);  
  
  if (kd.bDrawFibers) {
    Draw( tk.HF.fiberA(), 0,1,0 ); 
    Draw( tk.HF.fiberB(), 1,0,1 ); 
  }
  
  if (kd.bDrawVec) Draw(vec, 1, 1, 0 ); 

  if (kd.bDrawPnt) Draw(pnt,1,0,0);  
    
  if (kd.bDrawWrithe) {
    for(auto& i : wm ){
      i.drawVertices();
    }   
  }

  if (kd.bDrawTube) tube.drawElementsColor(); 
  
}


/*-----------------------------------------------------------------------------
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  App app; 
  app.start();  
  return 0;
}       
       
