/*
 * =====================================================================================
 *
 *       Filename:  xCrystal.cpp
 *
 *    Description:  3D crystallographic space group visualizer
 *
 *        Version:  1.0
 *        Created:  02/29/2016 16:09:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "horo_control.h" 
#include "horo_render.h"

#include "horo_vsr.hpp"

using namespace al;
using namespace vsr;
using namespace vsr::cga;

struct State{
  float time;
  gfx::Pose model;
  gfx::Pose camera;
  Point point;

  // add more state here

  
  
};

void drawState(State * state){
  State& s = *state;

  Draw(CXY(1));
}



/*-----------------------------------------------------------------------------
 *  CONTROL BONE
 *-----------------------------------------------------------------------------*/
struct ControlApp : ControlBone<State> {
 
  //Mouse Position
  Point mouse;

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

    state -> point = mouse;
    state -> camera = (gfx::Pose)scene.camera;
    state -> model = scene.model;
  }

  /*-----------------------------------------------------------------------------
   *  Draw Routines 
   *-----------------------------------------------------------------------------*/
  void onDraw(){
    mouse = calcMouse3D();  

    drawState(state);
  }
  
};




/*-----------------------------------------------------------------------------
 *  RENDER BONE
 *-----------------------------------------------------------------------------*/
struct RenderApp : OmniRenderBone<State>{

  /*-----------------------------------------------------------------------------
   *  Draw Loop Called Multiple Times Per Frame
   *-----------------------------------------------------------------------------*/
   void onDraw(Graphics& g){
     
     //MODEL pose to axis angle
     glPushMatrix();
       Vec4<> tr = state->model.quat().axan(); 
       glRotatef ( tr[3], tr[0], tr[1], tr[2]  );  
      
       drawState(state);   
            
    glPopMatrix();
   }

   /*-----------------------------------------------------------------------------
    *  Physics Called Once per Frame (do something with state)
    *-----------------------------------------------------------------------------*/
   void onAnimate(al_sec dt){
      auto& s = *(this->state);
      pose = gfx2al::pose( state->camera ); //conversion to al world 
   }

};  



/*-----------------------------------------------------------------------------
 *  Call Executable with ANY argument to launch control 
 *-----------------------------------------------------------------------------*/
int main (int argc, char *argv[]){

   if (argc > 1) {
     ControlApp app;
     app.start();
   } else {
     RenderApp app;
     app.start();
   }

    return 0;
}

