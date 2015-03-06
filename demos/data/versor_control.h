/*
 * =====================================================================================
 *
 *       Filename:  versor_control.h
 *
 *    Description:  some state info to pass around (example)
 *
 *        Version:  1.0
 *        Created:  03/05/2015 12:20:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */



#ifndef  versor_control_INC
#define  versor_control_INC

#include "gfx/gfx_scene.h"
#include "vsr_cga3D.h"

using namespace vsr;

struct State{

    Circle circle;
    gfx::Pose model;
    gfx::Pose pose;    

};

#endif   /* ----- #ifndef versor_control_INC  ----- */
