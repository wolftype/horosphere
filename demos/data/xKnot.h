/*
 * =====================================================================================
 *
 *       Filename:  xKnot.h
 *
 *    Description:  data for the knot 
 *
 *        Version:  1.0
 *        Created:  05/22/2015 19:49:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */



#ifndef  xKnot_INC
#define  xKnot_INC


#include "vsr/space/vsr_cga3D_op.h"

struct State{

  float P, Q, vel, tube_size, writhe;

  float theta, phi, rotVel;   

  float energy, energy_scale; 
  
  float size;

  int numcables;
                 
  float diameter;
  
  bool bAutoMode;
  bool bDrawRibbon, bDrawFibers, bDrawVec, bDrawTube, bDrawPnt;     
  bool bDrawWrithe, bUseEnergies, bFlow; 
  
  vsr::cga::Vec vec;
  vsr::cga::Point pnt;
  

};  


#endif   /* ----- #ifndef xKnot_INC  ----- */
