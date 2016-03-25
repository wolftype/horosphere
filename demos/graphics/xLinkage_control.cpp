/*
 * =====================================================================================
 *
 *       Filename:  xLinkage_control.cpp
 *
 *    Description:  uses versor app
 *
 *        Version:  1.0
 *        Created:  03/05/2015 18:27:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */



#include "vsr/vsr_app.h"
#include "vsr/form/vsr_linkages.h"
#include "vsr/draw/vsr_chain_draw.h"

#include "horo_vsr.hpp"

#include "data/linkage.h"
#include "Cuttlebone/Cuttlebone.hpp" 
 
using namespace vsr;
using namespace vsr::cga; 

using std::cout;
using std::endl;

struct MyApp : App {

    cuttlebone::Maker<State> maker;
    State * state;

    //MyApp() : App(), maker("192.168.10.255"){}
    MyApp() : App(), maker("192.168.10.255"){}
    //Local Structure (to be controlled by state data)
    Local local;

    Frame tframe, frame; // camera controller 
    bool bCameraLock,bCameraLockAbsolute, bCameraReset;
    float cameraDistance, cameraSpeed;

    void setup(){
 
      state = new State;
      memset(state, 0, sizeof(State));

      bindGLV();
      gui(state->numiter,"numiter",1,100);
      gui(state->numx,"numx",0,100);
      gui(state->numy,"numy",0,100);
      gui(state->d1,"d1",0,100);
      gui(state->d2,"d2",0,100);
      gui(state->theta,"theta",-TWOPI,TWOPI);
      gui(state->thetax,"thetax",-TWOPI,TWOPI);
      gui(state->thetay,"thetay",-TWOPI,TWOPI);
      gui(state->rot,"rot",-100,100);
      gui(state->amtx1,"amtx1",-1,1);
      gui(state->amtx2,"amtx2",-1,1);
      gui(state->amty1,"amty1",-1,1);
      gui(state->amty2,"amty2",-1,1);
      gui(state->link,"link",0,3);
      gui(state->bSubBennett,"subbennett");
      gui(bCameraLock,"CameraLock");
      gui(bCameraLockAbsolute,"CameraLockAbsolute");
      gui(bCameraReset,"CameraReset");
      gui(cameraDistance,"cameraDistance",-100,100);
      gui(cameraSpeed,"cameraSpeed",-100,100);

      state->numx=1; 
      state->link = 2;
      state->d1 = 3; 
      state->amtx1=state->amtx2=state->amty1=state->amty2=.5; 
      state->d2 = 3; 
      state->theta = PIOVERFOUR; 
      state->rot = .5; 
      state->thetax = -1; 
      state->thetay = -1;
  }
    
    virtual void step(double dt){}
  
    virtual void onAnimate();
    virtual void onDraw();
    virtual void updateLocal();
};


/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void MyApp::onAnimate(){

    static cuttlebone::Stats fps("linkage::step");

    state->pose = gfx2al::pose( scene.camera );     
    state->model = gfx::Pose( scene.model );     

    updateLocal();

    maker.set(*state); 

}

/*-----------------------------------------------------------------------------
 * UPDATE LOCAL PARAMETERS BASED ON SHARED STATE  
 *-----------------------------------------------------------------------------*/
//void MyApp::updateLocal(){
//    //clear
//    local.bennett.clear(); local.subBennett.clear();
//    //allocate
//    local.bennett = vector<Bennett>(state->numx);
//    if (state->bSubBennett) local.subBennett = vector<vector<Bennett>>( floor(state->numx/2.0) );
//
//    if (!local.bennett.empty()){
//      local.bennett[0].set(state->theta,state->d1,state->d2);
//      local.bennett[0]( state->rot );
//      bool bSwitch =true; int it=0;
//      for (int i=1;i<state->numx;++i){
//        local.bennett[i]=local.bennett[i-1].linkAt(2,state->thetax,state->amtx1,state->amtx2);
//
//        bSwitch = !bSwitch;
//        if (bSwitch){ if (state->bSubBennett) {
//          local.subBennett[it] = vector<Bennett>(state->numy);
//          
//          local.subBennett[it][0] = local.bennett[i].linkAt(1,state->thetay,state->amty1,state->amty2); 
//          for (int j=1;j<state->numy;++j){
//            local.subBennett[it][j]=local.subBennett[it][j-1].linkAt(2,state->thetay,state->amty1,state->amty2);
//          }
//          it++;
//        }}
//
//      }
//    }
//
//
//}
/*-----------------------------------------------------------------------------
 * UPDATE LOCAL PARAMETERS BASED ON SENT STATE  
 *-----------------------------------------------------------------------------*/
void MyApp::updateLocal(){
    
    local.bennett.clear(); local.subBennett.clear();
    local.bennett = vector<Bennett>(state->numx);
    if (state->bSubBennett) local.subBennett = vector<vector<Bennett>>(state->numx/2.0);

    if (!local.bennett.empty()){
    local.bennett[0].set(state->theta,state->d1,state->d2);
    local.bennett[0]( state->rot );
    }
    bool bSwitch =true; int it=0;
    for (int i=1;i<local.bennett.size();++i){
      local.bennett[i]=local.bennett[i-1].linkAt(2,state->thetax,state->amtx1,state->amtx2);

      bSwitch = !bSwitch;
      if (bSwitch){ if (state->bSubBennett) {
        if(local.subBennett.size() > it) { 
		      local.subBennett[it] = vector<Bennett>(state->numy);
        	if (!local.subBennett[it].empty() ) local.subBennett[it][0] = local.bennett[i].linkAt(1,state->thetay,state->amty1,state->amty2); 
        	for (int j=1;j<local.subBennett[it].size();++j){
          		local.subBennett[it][j]=local.subBennett[it][j-1].linkAt(2,state->thetay,state->amty1,state->amty2);
        	}
	}	
        it++;
      }}
    }
}



/*-----------------------------------------------------------------------------
 *  CALLED MANY TIMES PER FRAME: Draw all objects 
 *-----------------------------------------------------------------------------*/
void MyApp::onDraw() {
    
    gfx::GL::lightPos(1,1,1);
    gfx::GL::light();


    //draw and find average plane for orientation
    Point center;
    for (auto& i : local.bennett){
      Draw( (Chain)i, true, false, .5,.5,.5);
      DrawR( (Chain)i, .5,.5,.5);
      center += i[0].pos() / local.bennett.size();
    }

    for (auto& i : local.subBennett){ 
      for (auto& j: i) {
        Draw( (Chain)j, true, false, .5,.5,.5);
        DrawR( (Chain)j, .5,.5,.5);
        //center += i[0].pos();
      } 
    }
    center = Round::dls(center,1);
    Draw(center,1,0,0,.5);

    Plane plane;
    if (local.bennett.size()>5){
      Point pa = local.bennett[0][0].pos();
      Point pb = local.bennett[ floor(local.bennett.size() / 2.0) ][1].pos();
      Point pc = local.bennett[4][3].pos();
      plane = pa ^ pb ^ pc ^ Inf(1);
      plane = plane.unit();
      Draw(plane, 0,1,0);
    }


      auto orientation = (Vec( plane.dual() ) <= Vec::z )[0];
      cout << orientation << " " << plane.wt()  << endl;
      if ( fabs( plane.wt()) > 0 ){
       bool bDir = orientation > 0;
       cout << bDir << " DIR " << endl;
       tframe.pos() = Round::null(center).trs( plane.dual() * (bDir ? cameraDistance : -cameraDistance) ); 
       tframe.orient( center, true );
      }
  
    if (bCameraLockAbsolute){
      frame = tframe;
      scene.camera.set( frame.pos(), frame.quat() );
    } else if (bCameraLock){
      frame.relMotor( tframe ).print();
      frame.relTwist( tframe, cameraSpeed);
      scene.camera.set( frame.pos(), frame.quat() );
    } else if (bCameraReset){
      scene.camera.reset( 0,0,10 );
      Draw(tframe);
    }

//    frame.mot().print();
    scene.camera.print();

//    Point tpa = local.bennett[0][0].pos();
//    Point tpb = local.bennett[0][1].pos();
//    Point tpc = local.bennett[0][2].pos();
//    Point tpd = local.bennett[0][3].pos();
//
//    Plane tplane = tpa^tpb^tpc^Inf(1);
//    Draw(tplane,1,0,0);
}


/*-----------------------------------------------------------------------------
 *  Get Messages (e.g. from DeviceServer)
 *-----------------------------------------------------------------------------*/
/* void MyApp :: onMessage(osc::Message& m) { */   
/* } */

/*-----------------------------------------------------------------------------
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  MyApp app; 
  app.maker.start();
  app.start();  
  return 0;
}       
       

