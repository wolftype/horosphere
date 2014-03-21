/*
 * =====================================================================================
 *
 *       Filename:  xPolytopes.cpp
 *
 *    Description:  basic 3d polytopes
 *
 *        Version:  1.0
 *        Created:  03/20/2014 15:54:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "vsr/vsr_cga3D_op.h"   
#include "vsr/vsr_root.h"
#include "vsr/vsr_generic_draw.h"
#include "vsr/vsr_hull.h"

#include "horo_OmniApp.h"

using namespace vsr;
using namespace vsr::cga3D;

struct MyApp : OmniApp {

  Pnt mouse;
  Lin ray;

  float time;
  float p,q;

  float amt;

  // 3d convex hull
  ConvexHull<3> hull;
  // root system
  vector<NEVec<3>> rootSystem;

  MyApp() : OmniApp() {
    initGui();
  }

  void initGui(){
      glv.gui(amt,"amt",0,100);
      glv.gui(p,"p",0,10)(q,"q",0,10);
      p =4; q = 3;
  }

  virtual void init(){
      auto a = NEVec<3>::y;
      auto c = NEVec<3>::x;
      
      double tb = PIOVERTWO;
      double ta = PI/(int)p;
      double tc = PI/(int)q;

      //the good old spherical trig cosine rule
      double ca = cos(ta);
      double sa = sin(ta);
      double cb = cos(tb);
      double sb = sin(tb);
      double cc = cos(tc);
      double sc = sin(tc);

      double tA = acos( (ca-(cb*cc))/(sb*sc) );
      double tB = acos( (cb-(ca*cc))/(sa*sc) );
      double tC = acos( (cc-(ca*cb))/(sa*sb) );

      auto bivA = (a ^ c).rot( a.dual() * tA / 2.0 );
      auto bivC = (a ^ c).rot( c.dual() * -tC / 2.0 );

      auto b = (bivA.dual() ^ bivC.dual()).dual().unit();

      //Draw(a,1); Draw(b,1); Draw(c,1);

      rootSystem = Root::System(a, b, c);

      hull.initialFace(rootSystem);
      hull.altConvexPass(rootSystem);
      hull.closeHoles(amt);

  }

  virtual bool onFrame(){

    OmniApp::onFrame();



    return true;
  }

  virtual void onDraw(Graphics& g){ 

      for (auto& i : rootSystem) Draw(i);
      
      Draw(hull.graph);

  }

};

int main(){
  MyApp app;
  app.start();
  return 0;
}
