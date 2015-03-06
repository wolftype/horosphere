/*
 * =====================================================================================
 *
 *       Filename:  horo_glv_gui.h
 *
 *    Description:  allosphere versor interface
 *
 *        Version:  1.0
 *        Created:  01/20/2014 16:48:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), wolftype (gmail)
 *   Organization:  Media Arts and Technology Program, UCSB
 *
 * =====================================================================================
 */


#ifndef  al_glv_gui_INC
#define  al_glv_gui_INC


#include "vsr/vsr_gui.h"
#include "alloGLV/al_ControlGLV.hpp"

namespace al{
    //A GLV Object with built in GUI for easy prototyping
    struct GLVGui : public GLVDetachable {
        GLVGui() : GLVDetachable(), gui() { 
            *this << gui; 
            gui.colors().back.set(.3,.3,.3);
        }
        glv::Gui gui;
    };

    //non-detachable version
   struct GLVGui_ : public GLVBinding {

      GLVGui_() : GLVBinding(), gui(){
        *this << gui;
        gui.colors().back.set(.3,.3,.3);
      }
      glv::Gui gui;
   };
}
    
#endif   /* ----- #ifndef alv_interface_INC  ----- */
