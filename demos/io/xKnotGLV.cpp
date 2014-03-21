/*
 * =====================================================================================
 *
 *       Filename:  xKnotGLV.cpp
 *
 *    Description:  glv controller for knot
 *
 *        Version:  1.0
 *        Created:  03/18/2014 19:25:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#include "al_SharedData.h"
#include "data/vsr_knotData.h"
#include "horo_OSCApp.h"
#include "horo_GLVApp.h"

using namespace std;   
              
struct MyApp : public al::Window, public OSCApp, public GLVApp { 
  
  KnotData kd; 
  bool bUseGui;

  //listen to tablet interface, send from serving computer to main
  MyApp() : GLVApp(this), OSCApp(PORT_FROM_INTERFACE, PORT_FROM_SERVER_COMPUTER, MAIN_RENDERING_MACHINE) {

    initWindow();
    kd.buildGui(glv.gui);
    bUseGui = 1; 
  }

  void initWindow(){
    create( Window::Dim(400,600), "KNOT GLV", 60, Window::DEFAULT_BUF);
  }

  virtual bool onFrame(){
    
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    listen(); // listen for updates from tablet and other controllers 
     
    update(); // send gui info to client
  i
    onDraw(); 

    
    return true;
  }


  virtual void update(){
   if (bUseGui) SharedData::SendToMainFromServer( kd.bundle() );
  }

  virtual void start(){
    OSCApp::start();
    MainLoop::start();
  }

  virtual ~MyApp() {}

  virtual void onDraw() {   
  }

};
                                          

int main(int argc, char * argv[]) {
  MyApp app;
  app.start();
  return 0;
}
