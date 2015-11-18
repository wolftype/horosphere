/*
 * =====================================================================================
 *
 *       Filename:  xLinkage_ctl.cpp
 *    Description:  linkages with cuttlebone under the hood
 *
 *        Version:  1.0
 *        Created:  02/19/2015 15:26:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */





#include "vsr/vsr_cga3D_op.h"
#include "vsr/vsr_cga3D_draw.h"

#include "horo_GLVApp.h" 
#include "horo_OSCApp.h"
#include "horo_GxSync.h"

#include "vsr_linkages.h"
#include "vsr_chain_draw.h"

#include "gfx/gfx_mesh.h"

#include "data/linkage.h"
#include "allocore/io/al_App.hpp"
#include "Cuttlebone/Cuttlebone.hpp" 
 
using namespace al;
using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;

struct MyApp : App, GLVApp { //OSCApp

    cuttlebone::Maker<State,9000> maker;
    State * state;

    //Local Structure (to be controlled by state data)
    Local local;

    Frame frame; // camera controller

    MyApp()
    {
      init();
      initGui();
    }     

    virtual ~MyApp() {}
  
    void initGui(){
 
      state = new State;
      memset(state, 0, sizeof(State));

      glv.gui(state->numiter,"numiter",1,100);
      glv.gui(state->numx,"numx",0,100);
      glv.gui(state->numy,"numy",0,100);
      glv.gui(state->d1,"d1",0,100);
      glv.gui(state->d2,"d2",0,100);
      glv.gui(state->theta,"theta",-TWOPI,TWOPI);
      glv.gui(state->thetax,"thetax",-TWOPI,TWOPI);
      glv.gui(state->thetay,"thetay",-TWOPI,TWOPI);
      glv.gui(state->rot,"rot",-100,100);
      glv.gui(state->amtx1,"amtx1",-1,1);
      glv.gui(state->amtx2,"amtx2",-1,1);
      glv.gui(state->amty1,"amty1",-1,1);
      glv.gui(state->amty2,"amty2",-1,1);
      glv.gui(state->link,"link",0,3);
      glv.gui(state->bSubBennett,"subbennett");

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
    
    void init();

    virtual void step(double dt){}
  
    virtual void onAnimate(al_sec dt);
    virtual void onDraw(Graphics& g, const Viewpoint& v);
    virtual void updateLocal();
};

/*-----------------------------------------------------------------------------
 *  INITIALIZATION of windows etc
 *-----------------------------------------------------------------------------*/
void MyApp::init(){

    initWindow(Window::Dim(0, 0, 600, 400), "Linkage Control Center", 60);
    GLVApp::set(&window());
    initAudio(44100, 128, 2, 1);
}

/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void MyApp::onAnimate(al_sec dt){

    static cuttlebone::Stats fps("linkage::step");
    fps(dt);

    state->time += dt;
    state->pose = nav();     

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
void MyApp::onDraw(Graphics& g, const Viewpoint& v) {
    
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
       

