/*
 * =====================================================================================
 *
 *       Filename:  xVersor_Control.cpp
 *
 *    Description:  control omnirenderers in allosphere from a vsr_app (with cuttlebone)
 *
 *        Version:  1.0
 *        Created:  03/05/2015 12:12:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "vsr_cga3D_app.h"   
#include "Cuttlebone/Cuttlebone.hpp"
#include "data/versor_control.h"


using namespace vsr;
using namespace vsr::cga3D;

struct MyApp : App {

  //A Maker and A State
  cuttlebone::Maker<State,9000> maker;
  State * state;

  Circle circle = CXY(1);
 
  //Some Variables
  bool bReset = false;
  float amt = 0;

  /*-----------------------------------------------------------------------------
   *  Setup Variables
   *-----------------------------------------------------------------------------*/
  void setup(){
    ///Bind Gui
    bindGLV();
    ///Add Variables to GUI
    gui(amt,"amt",-100,100)(bReset,"bReset");

    ///initialize state
    state = new State;    
    memset(state, 0, sizeof(State));

    objectController.attach(&circle);
  }

  /*-----------------------------------------------------------------------------
   *  Loop
   *-----------------------------------------------------------------------------*/
  void onAnimate(){
    state->pose = (Pose)( scene.camera );
    state->model = (Pose)scene.model;
    state->circle = circle;
    maker.set(*state);
  }

  /*-----------------------------------------------------------------------------
   *  Draw Routines 
   *-----------------------------------------------------------------------------*/
  void onDraw(){
 
    Draw(circle);
 
  }
  
};


int main(){
                             
  MyApp app;
  app.maker.start();
  app.start();

  return 0;

}
