/*
 * =====================================================================================
 *
 *       Filename:  audiotest.cpp
 *
 *    Description:  test AudioApp
 *
 *        Version:  1.0
 *        Created:  02/26/2014 16:19:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "horo_AudioApp.h"
#include "horo_AudioProcess.h"


struct App : public AudioApp {

  vector< AudioProcess * > ap;

  App() : AudioApp() {
    
    ap.push_back( &mScheduler.add<WindSound>() ); 
    ap.back() -> mix = .065; 
    ap.back() -> initGui(glv.gui);
  
  }
  
  virtual void update(){
    ap.back() -> update();
  }

};

/*-----------------------------------------------------------------------------
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  App app; 
  app.start();  
  return 0;
}  
