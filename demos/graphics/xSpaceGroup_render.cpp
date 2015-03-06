/*
 * =====================================================================================
 *
 *       Filename:  xSpaceGroup_render.cpp
 *
 *    Description:  renderer of crystal groups
 *
 *        Version:  1.0
 *        Created:  03/05/2015 19:01:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


//#include "data/spacegroup_control.h"
#include "vsr_cga3D.h"
#include "vsr_cga3D_draw.h"
#include "vsr_group.h"

#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "horo_vsr.hpp"

using namespace al;
using namespace std;
using namespace vsr;

struct State{

//  gfx::Pose pose;
//  gfx::Pose model;

  float amt=0;

  float p=2;
  float q=2;
  bool pbar=false;
  bool qbar=false; 
  bool pqbar=false;
  
  //Point point = Ro::null(0,0,0);
 

};


//void drawState(State * state){
//
//     State& s = *state;
//
//     Point point = Ro::null(1,1,1);
//         
//     PointGroup3D<vsr::Vec> pg(s.p,s.q,s.pbar,s.qbar,s.pqbar);
//
//   // if (pg.ops.size()>2){
//      for (int i=0;i<pg.ops.size();++i){
//        Draw(pg.ops[i],(float)i/pg.ops.size(),0,1-(float)i/pg.ops.size());
//      }
//   // }
//    for (auto& i : pg.sops){
//     // Draw(Biv(i));
//    }
//    Draw(pg.a,1,0,0); Draw(pg.b,0,1,0); Draw(pg.c,0,0,1);
//    
//    auto res = pg( point ^ point.trs(.2,0,.1) );
//    auto res2 = pg( point ^ point.trs(0,.5,.2) );
//    //auto res3 = pg( Ro::round( Ro::dls(mouse,.2), Biv(.1,.2,.3).runit() ) ); // mouse.trs(0,.5,.2) );
//
//
//   for (int i=0;i<res.size();++i) {
//     Draw(res[i],(float)i/res.size(),0,1-(float)i/res.size());
//     Draw(res2[i],(float)i/res.size(),1,1-(float)i/res.size());
//    // Draw(res3[i],(float)i/res.size(),1,1-(float)i/res.size());
//    }
//   // cout << pg.ops.size() << " " << pg.sops.size() << endl;
//
//}


/*-----------------------------------------------------------------------------
 *  RENDERING
 *-----------------------------------------------------------------------------*/
using namespace al;

struct RenderApp : OmniStereoGraphicsRenderer {
  
  cuttlebone::Taker<State, 9000> taker;
  State* state;
  
  RenderApp() {
    state = new State;
  }

  virtual ~RenderApp() {}

  virtual void onAnimate(double dt) {
    int popCount = taker.get(*state);
    //pose = gfx2al::pose( state->pose ); //conversion
  }

  virtual void onDraw(Graphics& g) {
     //model 
    glPushMatrix();
     // Vec4<> tr = state->model.quat().axan(); 
    //  glRotatef ( tr[3], tr[0], tr[1], tr[2]  );  
      
    //  drawState(state);   
            
    glPopMatrix();
  }

};


int main (int argc, char *argv[]){

    RenderApp app;
    app.taker.start();
    app.start();

    return 0;
}


