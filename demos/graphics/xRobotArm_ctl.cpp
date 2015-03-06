/*
 * =====================================================================================
 *
 *       Filename:  xRobotArm_ctl.cpp
 *
 *    Description:  RobotArm 3DOF Demo
 *
 *        Version:  1.0
 *        Created:  03/05/2015 15:45:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "vsr/vsr_cga3D_op.h"
#include "vsr/vsr_cga3D_draw.h"
#include "vsr/vsr_chain_draw.h"
#include "vsr/vsr_knot.h"

#include "horo_GLVApp.h" 
#include "horo_OSCApp.h"
#include "horo_GxSync.h"

#include "Cuttlebone/Cuttlebone.hpp" 

#ifdef CTL_MAKER
#include "allocore/io/al_App.hpp"
#else
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#endif

 
using namespace al;
using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;

/*-----------------------------------------------------------------------------
 *  Shared State
 *-----------------------------------------------------------------------------*/
struct State {

   float linkLength;
   float lineWidth;

   al::Pose pose;
   Frame  targetFrame;        ///<-- a target point

};


/*-----------------------------------------------------------------------------
 *  Local state
 *-----------------------------------------------------------------------------*/
struct Local {
  
  Chain k = Chain(5);           ///<-- a kinematic chain

  Frame     baseFrame;          ///<-- base pose of robot arm
  Frame     secondFrame;
   
  // Planes will intersect to define a line which intersects with final sphere
  vsr::Plane     rotationPlane;      ///<-- Plane formed by Y axis of base and Target Point
  DualPlane targetPlane;        ///<-- Plane Through Target Parallel to Ground
  DualLine  targetLine;         ///<-- Line Through Target
  DualSphere targetSphere;      ///<-- Sphere around Target
  Pair      finalJointPair;     ///<-- Pair of points for locaiton of final joint
 
  DualSphere sphereA, sphereB;  ///<-- Spheres around joints
  Circle    middleCircle;       ///<-- Middle Circle meet
  Pair      middleJointPair;    ///<-- Pair of points for location of middle joint   

  //calculations based on state parameters
  void calc(const State& s){
    
    //known positions
    k[0] = Frame();
    k[1] = Frame(0,s.linkLength,0);
    k[4] = s.targetFrame;
    
    //known spheres (around first joint and target joint)
    targetSphere = Ro::sphere( s.targetFrame.pos(), s.linkLength);
    
    //known planes and their intersection
    rotationPlane = k[0].ly() ^ s.targetFrame.pos();
    targetPlane = k[0].dxz();
    targetLine = targetPlane ^ rotationPlane.dual();

    //finalJoint Position
    finalJointPair = (targetLine ^ targetSphere).dual();
    k[3] = Frame( Ro::split( finalJointPair, false) ); 

    //spheres
    sphereA = Ro::sphere( k[1].pos(), s.linkLength);
    sphereB = Ro::sphere( k[3].pos(), s.linkLength);
    middleCircle = (sphereB ^ sphereA).dual();
    middleJointPair = (rotationPlane.dual() ^ middleCircle.dual()).dual();

    k[2] = Frame( Ro::split(middleJointPair,true) );

    //positions are set, now calculate joint rotations
    Rot r1 = Gen::ratio( vsr::Vec::z, rotationPlane.dual().runit() );
    k[0].rot( r1 );
    //for all the other frames, calculate joint rotations and link lengths from current positions
		k.calcJoints(1); 
		k.links();
  }

  void draw(){
 
    Draw(k);
     
    Draw(rotationPlane,0,1,0);
    Draw(targetPlane,0,.5,1);
    Draw(targetLine,1,1,0);
    Draw(middleCircle,0,1,1);
    Draw(sphereA,1,.2,.2,.4);
    Draw(sphereB,1,.2,.2,.4);
    Draw(targetSphere,1,.0,.0,.4);

  }
};


#ifdef CTL_MAKER
struct MyApp : App, GLVApp {                                                      //<-- Maker Code
#else
struct MyApp : OmniStereoGraphicsRenderer {                                               //<-- Taker Code
#endif

    #ifdef CTL_MAKER
    cuttlebone::Maker<State,9000> maker;                                          //<-- Maker Code
    #else       
    cuttlebone::Taker<State,9000> taker;                                          //<-- Taker Code
    #endif

    State * state;
    Local local;

    //Some things for the simulator to use internally
    TorusKnot tk = TorusKnot(3,2);
    Frame frame = Frame(5,0,0); //Point Controller
    float amt,fx,fy = 0.0;
    bool bReset;
    bool bDrawTarget, bDrawPlaneA, bDrawPlaneB, bDrawLine, bDrawPairA, bDrawSpheres, bDrawCircle,bDrawPairB;
     
    MyApp()
    {
      init();
    }     

    void init();
  
    
    /*-----------------------------------------------------------------------------
     *  Initialze Gui on Maker Machine
     *-----------------------------------------------------------------------------*/
#ifdef CTL_MAKER
     void initGui(){
      State& s = *state;
      glv.gui(s.linkLength, "LinkLength", 1,10);
      glv.gui(s.lineWidth, "lineWidth", 1,10);
      glv.gui(amt, "amt",-10,10);
      glv.gui(fx, "fx",-10,10);
      glv.gui(fy, "fx",-10,10);
      glv.gui(bReset, "bReset");
      glv.gui(bDrawTarget, "bDrawTargetSphere");
      glv.gui(bDrawPlaneA, "bDrawPlaneParallelToGround");
      glv.gui(bDrawPlaneB, "bDrawPlaneOfRotation");
      glv.gui(bDrawLine, "bDrawLineOfIntersection");
      glv.gui(bDrawPairA, "bDrawPairIntersectionA");
      glv.gui(bDrawSpheres, "bDrawSpheresOfPossibilty");
      glv.gui(bDrawCircle, "bDrawCircleOfPossibilty");
      glv.gui(bDrawPairB, "bDrawPairIntersectionB");
      s.linkLength = 5.0;
      s.lineWidth=3;
    }
 #endif
    
    virtual void step();
    virtual void onAnimate(al_sec dt);
    virtual void onDraw(Graphics& g, const Viewpoint& v);
    virtual void updateLocal();
    
    virtual ~MyApp() {}
};

/*-----------------------------------------------------------------------------
 *  INITIALIZATION of windows etc
 *-----------------------------------------------------------------------------*/
void MyApp::init(){
   state = new State;
   memset(state, 0, sizeof(State));
   
   #ifdef CTL_MAKER                                                             //<-- Maker Code
   initWindow(Window::Dim(0, 0, 600, 400), "Linkage Control Center", 60);
   GLVApp::set(&window());
   initAudio(44100, 128, 2, 1);
   initGui();
   #endif
}

void MyApp::step(){
   if (bReset) { bReset = false; frame = Frame(fx,fy,0); }

   frame.boost( tk.par() * amt ); 
   state -> targetFrame = frame;
   state -> pose = nav();

}

/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void MyApp::onAnimate(al_sec dt){

    #ifdef CTL_MAKER                                                            //<-- Maker Code
    static cuttlebone::Stats fps("linkage::step");
    fps(dt);

    //state->time += dt;
    //state->pose = nav();     
    step();
    maker.set(*state); 
    #else                                                                       //<-- Taker Code
    int popCount = taker.get(*state);
    pose = state->pose;
    #endif

    updateLocal();
}

/*-----------------------------------------------------------------------------
 * UPDATE LOCAL PARAMETERS BASED ON STATE  
 *-----------------------------------------------------------------------------*/
void MyApp::updateLocal(){
   local.calc(*state);
}

/*-----------------------------------------------------------------------------
 *  CALLED MANY TIMES PER FRAME: Draw all objects 
 *-----------------------------------------------------------------------------*/
void MyApp::onDraw(Graphics& g, const Viewpoint& v) {
    
    State& s = *state;

    gfx::GL::lightPos(1,1,1);
    gfx::GL::light();
    glLineWidth(s.lineWidth);

    local.draw();

}


/*-----------------------------------------------------------------------------
 *  Get Messages (e.g. from iPad)
 *-----------------------------------------------------------------------------*/
/* void MyApp :: onMessage(osc::Message& m) { */   
/* } */

/*-----------------------------------------------------------------------------
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  MyApp app; 
  #ifdef CTL_MAKER                                                              //<-- Maker Code
  app.maker.start();
  #else
  app.taker.start();                                                            //<-- Taker Code
  #endif
  app.start();  
  return 0;
}       
       

