/*
 * =====================================================================================
 *
 *       Filename:  xEmpty.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2015 16:22:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef  data_empty_INC
#define  data_empty_INC

#include "allocore/spatial/al_Pose.hpp"
#include "Cuttlebone/Cuttlebone.hpp"  
#include "gfx/gfx_scene.h"

//state sent from simulator to renderer
struct State{

  float time=0; 
  al::Pose pose;
  gfx::Pose model;

};

//shared by simulator and renderer to build up local structures
struct Local{

};


#endif   /* ----- #ifndef linkage_INC  ----- */
