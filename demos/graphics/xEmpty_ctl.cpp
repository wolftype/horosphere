/*
 * =====================================================================================
 *
 *       Filename:  xEmpty_ctl.cpp
 *
 *    Description:  an empty cuttlebone app to build from
 *                  #define -D CTL_MAKER for simulator
 *
 *        Version:  1.0
 *        Created:  03/05/2015 14:16:10
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

};


#ifdef CTL_MAKER
struct MyApp : App, GLVApp {                                                      //<-- Maker Code
#else
struct MyApp : OmniStereoRenderer {                                               //<-- Taker Code
#endif

    #ifdef CTL_MAKER
    cuttlebone::Maker<State,9000> maker;                                          //<-- Maker Code
    #else       
    cuttlebone::Taker<State,9000> taker;                                          //<-- Taker Code
    #endif

    State * state;

    Frame frame; // camera controller

    MyApp()
    {
      init();
    }     

    void init();
  
    
    /*-----------------------------------------------------------------------------
     *  Initialze Gui on Maker Machine
     *-----------------------------------------------------------------------------*/
    void initGui(){
 
    }
    

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

/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void MyApp::onAnimate(al_sec dt){

    #ifdef CTL_MAKER                                                            //<-- Maker Code
    static cuttlebone::Stats fps("linkage::step");
    fps(dt);

    //state->time += dt;
    //state->pose = nav();     

    updateLocal();
    maker.set(*state); 
    #else                                                                       //<-- Taker Code
    taker.set(*state);
    updateLocal();
    #endif

}

/*-----------------------------------------------------------------------------
 * UPDATE LOCAL PARAMETERS BASED ON STATE  
 *-----------------------------------------------------------------------------*/
void MyApp::updateLocal(){
   
}

/*-----------------------------------------------------------------------------
 *  CALLED MANY TIMES PER FRAME: Draw all objects 
 *-----------------------------------------------------------------------------*/
void MyApp::onDraw(Graphics& g, const Viewpoint& v) {
    
    gfx::GL::lightPos(1,1,1);
    gfx::GL::light();

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
       

