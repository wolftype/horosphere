/*
 * =====================================================================================
 *
 *       Filename:  horo_GLVApp.cpp
 *
 *    Description:  glv interface
 *
 *        Version:  1.0
 *        Created:  03/20/2014 12:57:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#include "GLV/glv.h"  
#include "GLV/glv_binding.h"
#include "GLV/glv_util.h" 
#include "GLV/glv_layout.h"

#include "horo_glv_gui.h"
//"gfx/util/glv_gui.h"//

#include "allocore/al_Allocore.hpp"

struct GLVApp {

  al::GLVGui glv;
  al::GLVGui_ glv_;

  al::Window * win;
  
  //pass in a window to the constructor
  GLVApp(al::Window * w = NULL) : win(w) {
    if (win) initGLV();
  }

  void set(al::Window * w ) {
    win = w;
    initGLV();
    
  }

 virtual void initGLV(){
    glv.parentWindow(*win);
    glv.gui.colors().back.set(.3,.3,.3);
    glv.gui.colors().border.set(1,0,0);
    glv.gui.enable( glv::DrawBorder );

 //   glv_.bindTo(*win);
 //   glv_.gui.colors().back.set(.3,.3,.3);
 //   glv_.gui.colors().border.set(1,0,0);
 //   glv_.gui.enable( glv::DrawBorder );
  }

  void toggle(){
    glv.gui.toggle( glv::Property::Visible );
  }


};
