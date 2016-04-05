/// the knot

#include "vsr/vsr_app.h"
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
