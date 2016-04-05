/// the knot


#include "space/vsr_cga3D_op.h"               //<-- conformal 3D types and basic operations

#include "draw/vsr_cga3D_draw.h"              //<-- fixed pipeline draw routines
#include "draw/vsr_cga3D_render.h"            //<-- programmable pipeline draw routines

#include "hsRender.hpp"
#include "../demos/data/xKnotData3.hpp"


using namespace hs;
using namespace vsr;
using namespace vsr::cga;

int main(int argc, char * argv[]) {

  RenderApp<Knot> app;
  app.start();

  return 0;
}
