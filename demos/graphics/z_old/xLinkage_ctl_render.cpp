/*
 * =====================================================================================
 *
 *       Filename:  xLinkage_ctl_render.cpp
 *
 *    Description:  rendering state sent from simulator
 *
 *        Version:  1.0
 *        Created:  03/05/2015 08:51:35
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

#include "horo_GLVApp.h" 
#include "horo_OSCApp.h"
#include "horo_GxSync.h"

#include "vsr_linkages.h"
#include "vsr_chain_draw.h"

#include "gfx/gfx_mesh.h"

#include "data/linkage.h"

#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"

using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;

struct MyApp : OmniStereoGraphicsRenderer  { 

    cuttlebone::Taker<State, 9000> taker;
    State* state;

    //Local Structure (to be filled with sent state data)
    Local local;

    MyApp()
    {
      init();
    }     

    virtual ~MyApp() {}
      
    
    /*-----------------------------------------------------------------------------
     *  Initialize
     *-----------------------------------------------------------------------------*/
    void init(){
      state = new State;
      memset(state, 0, sizeof(State));

    }
    
    /*-----------------------------------------------------------------------------
     *  Start
     *-----------------------------------------------------------------------------*/
    virtual void start() {
     taker.start();                        // non-blocking
     OmniStereoGraphicsRenderer::start();  // blocks
    }

    virtual void step(double dt){}
  
    virtual void onAnimate(al_sec dt);
    virtual void onDraw(Graphics& g);//const Viewpoint& v);   
    virtual void updateLocal();

};

/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void MyApp::onAnimate(al_sec dt){
    int popCount = taker.get(*state);
    pose = state->pose;

    updateLocal();
}

/*-----------------------------------------------------------------------------
 * UPDATE LOCAL PARAMETERS BASED ON SENT STATE  
 *-----------------------------------------------------------------------------*/
void MyApp::updateLocal(){
    
    local.bennett.clear(); local.subBennett.clear();
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
void MyApp::onDraw(Graphics& g) {


        
    
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
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  MyApp app; 
  app.start();  
  return 0;
}       
       

