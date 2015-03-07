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



#include "vsr/vsr_cga3D_app.h"
#include "vsr_linkages.h"
#include "vsr_chain_draw.h"

#include "horo_vsr.hpp"

#include "data/linkage.h"
#include "Cuttlebone/Cuttlebone.hpp" 
 
using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;

struct MyApp : App {

    cuttlebone::Maker<State> maker;
    State * state;

    //Local Structure (to be controlled by state data)
    Local local;

    Frame frame; // camera controller 

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
void MyApp::updateLocal(){
    //clear
    local.bennett.clear(); local.subBennett.clear();
    //allocate
    local.bennett = vector<Bennett>(state->numx);
    if (state->bSubBennett) local.subBennett = vector<vector<Bennett>>(state->numx/2.0);

    local.bennett[0].set(state->theta,state->d1,state->d2);
    local.bennett[0]( state->rot );
    bool bSwitch =true; int it=0;
    for (int i=1;i<state->numx;++i){
      local.bennett[i]=local.bennett[i-1].linkAt(2,state->thetax,state->amtx1,state->amtx2);

      bSwitch = !bSwitch;
      if (bSwitch){ if (state->bSubBennett) {
        local.subBennett[it] = vector<Bennett>(state->numy);
        
        local.subBennett[it][0] = local.bennett[i].linkAt(1,state->thetay,state->amty1,state->amty2); 
        for (int j=1;j<state->numy;++j){
          local.subBennett[it][j]=local.subBennett[it][j-1].linkAt(2,state->thetay,state->amty1,state->amty2);
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

    for (int i=0;i<state->numx;++i){
      Draw( (Chain)local.bennett[i], true, false, .5,.5,.5);
      DrawR( (Chain)local.bennett[i], .5,.5,.5);
    }

    for (auto& i : local.subBennett){ 
      for (auto& j: i) {
        Draw( (Chain)j, true, false, .5,.5,.5);
        DrawR( (Chain)j, .5,.5,.5);
      } 
    }
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
  app.maker.start();
  app.start();  
  return 0;
}       
       

