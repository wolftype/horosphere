/*
 * =====================================================================================
 *
 *       Filename:  xBasic.cpp
 *
 *    Description:  test
 *
 *        Version:  1.0
 *        Created:  01/20/2014 15:23:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), wolftype (gmail)
 *   Organization:  Media Arts and Technology Program, UCSB
 *
 * =====================================================================================
 */

#include "vsr/vsr_cga3D_op.h"

//#include "vsr/vsr_generic_op.h"

using namespace vsr;
using namespace vsr::cga3D;

int main(){
  
  Pnt pt = Ro::null(1,0,0);
  pt.print();

  auto cir = Par() ^ Ro::null(0,1,0);//PX(1) ^ PX(-1) ^ PY(1);
  
  auto spar = Par().sp( Gen::rot(Biv::xy*.5) );

  spar.print();

  cir.print();

  return 0;
}
