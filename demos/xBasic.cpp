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

int main(){
  
  vsr::Pnt pt = PT(1,0,0);
  pt.print();

  vsr::Circle cir = CXY(1);

  cir.print();

  return 0;
}
