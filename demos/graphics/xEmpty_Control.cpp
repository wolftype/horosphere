/*
 * =====================================================================================
 *
 *       Filename:  xEmpty_Control.cpp
 *
 *    Description:  base controller app
 *
 *        Version:  1.0
 *        Created:  05/22/2015 16:17:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#include "horo_control.h" 
#include "data/xEmpty.h"  

using namespace vsr;
using namespace vsr::cga;

struct MyApp : ControlApp<State> {
 
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
    
  }

  void onAnimate(){
    auto &s = *(this->state);
    s.time += .01;
  }

  /*-----------------------------------------------------------------------------
   *  Draw Routines 
   *-----------------------------------------------------------------------------*/
  void onDraw(){
  
  }
  
};


int main(){
                             
  MyApp app;
  app.start();

  return 0;

}
