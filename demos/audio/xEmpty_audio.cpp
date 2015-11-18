/*
 * =====================================================================================
 *
 *       Filename:  xEmpty_audio.cpp
 *
 *    Description:  gets state from a simulator and renders audio
 *
 *        Version:  1.0
 *        Created:  05/22/2015 19:16:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#include "data/xEmpty.h" //<-- must include vsr gfx first so glew is included before gl

using namespace al;

struct MyApp : AudioBone<State>{

  /*-----------------------------------------------------------------------------
   *  Draw Loop Called Multiple Times Per Frame
   *-----------------------------------------------------------------------------*/
   void onDraw(Graphics& g){
   }

   /*-----------------------------------------------------------------------------
    *  Physics Called Once per Frame (do something with state)
    *-----------------------------------------------------------------------------*/
   void onAnimate(al_sec dt){
      auto& s = *(this->state);
      cout << s.time << endl;
   }

};  



/*-----------------------------------------------------------------------------
 *  MAIN LOOP
 *-----------------------------------------------------------------------------*/
int main(){
  MyApp app;
  app.start();
  return 0;
}
