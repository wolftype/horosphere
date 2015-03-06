/*
 * =====================================================================================
 *
 *       Filename:  xVersor_Render.cpp
 *
 *    Description:  basic renderer of versor content (allosphere)
 *
 *        Version:  1.0
 *        Created:  03/05/2015 12:50:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "data/versor_control.h"
#include "vsr_cga3D.h"
#include "vsr_cga3D_draw.h"

#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "horo_vsr.hpp"

using namespace al;
using namespace std;
using namespace vsr;


struct MyApp : OmniStereoGraphicsRenderer {
  
  cuttlebone::Taker<State, 9000> taker;
  State* state;
  
 // Circle cxy = CXY(1);

  MyApp() {
    state = new State;
  }

  virtual ~MyApp() {}

  virtual void onAnimate(double dt) {
    int popCount = taker.get(*state);
    pose = gfx2al::pose( state->pose ); //conversion
  }

  virtual void onDraw(Graphics& g) {
     //model 
    glPushMatrix();
      Vec4<> tr = state->model.quat().axan(); 
      glRotatef ( tr[3], tr[0], tr[1], tr[2]  );  
      
      Draw( state->circle );
     // Draw(cxy);
    
    glPopMatrix();
  }

};


int main() { 
    MyApp app;
    app.taker.start();
    app.start(); 
}


/*-----------------------------------------------------------------------------
 *  Optionally ...
 *-----------------------------------------------------------------------------*/
 //   shader().uniform("lighting", 0.0);
 //   omni().clearColor() = state->backgroundColor;
 //   lens().near(state->nearClip);
 //   lens().far(state->farClip);
 //   lens().eyeSep(state->eyeSeparation);

