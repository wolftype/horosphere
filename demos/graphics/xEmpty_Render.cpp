/*
 * =====================================================================================
 *
 *       Filename:  xEmpty_Render.cpp
 *
 *    Description:  subclass omnistereorender with ctl taker 
 *
 *        Version:  1.0
 *        Created:  05/22/2015 16:08:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#include "data/xEmpty.h" //<-- must include vsr gfx first so glew is included before gl
#include "horo_render.h"

using namespace al;

struct MyApp : OmniRenderBone<State>{

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
