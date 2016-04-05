/// the knot
#include "hsSimulator.hpp"
#include "hsRender.hpp"
#include "vsr/form/vsr_knot.h"
#include "vsr/form/vsr_shapes.h"

using namespace hs;
using namespace vsr;
using namespace vsr::cga;
/*-----------------------------------------------------------------------------
 *  DATA
 *-----------------------------------------------------------------------------*/
 struct BasicState{

   float time;

   gfx::Pose model;
   gfx::Pose camera;

   cga::Point mouse;

 };

struct Knot{

  //Global Parameters to pass around
  struct State  {
    BasicState mScene;

    //Orbit Parameters
    float P, Q;                             //<-- Knot Ratio
    float theta, phi;                       //<-- Sph Coord of Vec
    float vel, rotvel;                      //<-- Velocity of Orbit
    float ecc;                              //<-- Hyperbolicness of orbit

    float tube_size, writhe;
    float energy, energy_scale;
    float size;

    int numcables;

    bool bAutoMode;
    bool bUseEnergies, bFlow;

    //Drawing Parameters
    bool bDrawRibbon, bDrawFibers, bDrawVec, bDrawTube, bDrawPnt;
    bool bDrawWrithe;

    //Hopf Vec Direction and Point on Orbit
    cga::Vec vec;                              //<-- Hopf Vec
    cga::Point pnt = Construct::point(3,0,0);  //<-- Point Along Orbit

  } mState;

  //PQ Function
  TorusKnot tk;

  //Meshes
  gfx::Mesh tm;             // Main Torus Knot Mesh
  gfx::Mesh tube;           // Tube Mesh

  vector<gfx::Mesh> wm;     // Some number of writhe Meshes;

  void setup(){
    State& s = mState;

    //Defaults
    s.P = 3; s.Q = 2;
    s.vel = .015;
    s.writhe = 10;
    s.numcables = 2;
    s.tube_size = 1.;
    s.rotvel = .01;

    s.bDrawWrithe = true;
    s.bFlow = true;
    s.bAutoMode = true;

    s.energy_scale = 20;

    s.vec = vsr::cga::Vec(0,1,0);
    s.pnt = Round::null(3,0,0);
  }




  /*-----------------------------------------------------------------------------
   *  Update Local Data (called on all Apps)
   *-----------------------------------------------------------------------------*/
  void updateLocal(){
    State& s = mState;

    //Knot Settings
    tk.HF.vec() = s.vec;
    tk.P = s.P;
    tk.Q = s.Q;
    tk.amt = s.vel;

    //Knot Calculation
    s.energy = tk.calc0(s.pnt);

    //Mesh Data
    tm.clear();
    wm.clear();
    tube.clear();

    //Tube Mesh
    int res = 5;
    int num = tk.cir.size();
    tube = Shape::Skin( tk.cir, num, true, res );

    //Tube Color by Energy
    for (int i = 0; i<num;++i){
      float c = tk.energies[i];
      c = CLAMP(c,0,1);
      float pt = TWOPI * (float)i/num;
      for (int j=0; j<res;++j){
        gfx::Vec4f tv( (1+sin(pt))/2.0,.2,(1+cos(pt))/2.0,.5);
        tube[i + j * num ].Col.set(c*s.energy_scale*tv[0], tv[1], tv[2] + ( (1-c) * s.energy_scale ), tv[3]);
      }
    }

  }


  /*-----------------------------------------------------------------------------
   *  Update Global State (called from Control App)
   *-----------------------------------------------------------------------------*/
  void updateGlobal(){
    State& s = mState;

    /// Orbit of Point
    if(s.bFlow) s.pnt = Round::loc( s.pnt.spin( tk.bst() ) );

    /// Polar Position of Hopf Links, either automatically by tracking point
    /// or manually by setting theta / phi.
    vsr::cga::Vec tvec;
    if (s.bAutoMode){
     tvec = vsr::cga::Vec(s.pnt).unit();
    } else {
     tvec =  vsr::cga::Vec::x.spin( Gen::rot(s.theta, s.phi) );
    }
    /// Slow Slerp to Polar position
    Rot r = Gen::rot( Gen::log( Gen::ratio(tvec, s.vec) ) * s.rotvel );
    s.vec = s.vec.spin( r );


    /// Tie Writhe to energy
    if (s.bUseEnergies){
      s.writhe = s.energy / s.energy_scale;
    }

  }

  /*-----------------------------------------------------------------------------
   *  Draw State (called from All Apps)
   *-----------------------------------------------------------------------------*/
  void onDraw(){
    State& s = mState;

    glPointSize(10);

    if (s.bDrawFibers) {
      Draw( tk.HF.fiberA(), 0,1,0 );
      Draw( tk.HF.fiberB(), 1,0,1 );
    }

    if (s.bDrawVec)
    Draw(s.vec, 1, 1, 0 );

    if (s.bDrawPnt)
    Draw(s.pnt,1,0,0);

    if (s.bDrawTube)
    tube.drawElementsColor();

    //
    //    if (s.bDrawWrithe) {
    //      for(auto& i : wm ){
    //        i.drawVertices();
    //      }
    //    }

  }

  void onMessage(al::osc::Message& m){
      State& s = mState;
      auto ap = m.addressPattern();
      float tmp;
      if (ap=="/bAutoMode") {
        m >> tmp;
        s.bAutoMode = tmp;
      } else if (ap == "/bFlow") {
        m >> tmp;
        s.bFlow = tmp;
      } else if (ap == "/P") {
        m >> tmp;
        s.P = tmp;
      } else if (ap == "/Q") {
        m >> tmp;
        s.Q = tmp;
      }
  }

};

namespace hs{
///Named Boolean Parameters of Knot
template<> template<> void Param<bool>::specify(Knot& k){
  mData = {
    {"bDrawFibers",&k.mState.bDrawFibers},
    {"bDrawTube",&k.mState.bDrawTube},
    {"bAutoMode",&k.mState.bAutoMode},
    {"bDrawVec",&k.mState.bDrawVec},
    {"bDrawPnt",&k.mState.bDrawPnt},
    {"bDrawWrithe",&k.mState.bDrawWrithe},
    {"bFlow",&k.mState.bFlow}
  };
}

template<> template<> void Param<float>::specify(Knot& k){
  mData = {
    {"P",&k.mState.P,1,10},
    {"Q",&k.mState.Q,1,10}
  };
}
}
struct ControlApp : hs::Simulator<Knot> {

  ControlApp(const char * ip) : hs::Simulator<Knot>(ip) {}

  FMSynth * fm;
  WindSound * ws;
  Harmonics * hs;

  virtual void setup() override {
    bindGLV();

    auto& tfm = audio.mScheduler.add<FMSynth>();
    auto& tws = audio.mScheduler.add<WindSound>();
    auto& ths = audio.mScheduler.add<Harmonics>();

    glui.bind<AudioParam>(tfm,gui);
    glui.bind<AudioParam>(tws,gui);
    glui.bind<AudioParam>(ths,gui);

    glui.bind<Param<bool>>(mParam,gui);
    glui.bind<Param<float>>(mParam,gui);

    fm = &tfm; ws = &tws; hs = &ths;
  }

  /// Here is where we can tie Audio Parameters to Global State
  virtual void onAnimate() override {
    auto &s = this->mParam.mState.mScene;
    s.time += .01;
    s.mouse = mouse;
    s.camera = (gfx::Pose)scene.camera;
    s.model = scene.model;

    mParam.updateLocal();
    mParam.updateGlobal();

    updateAudio();
  }

  void updateAudio(){
    auto &kd = this->mParam.mState;

    auto diameter = Round::size( mParam.tk.cir[0], true );
    ws->freq = 330. + ( 110. * ( 10 * diameter ) );
    ws->width = 10 + ( 8000. * diameter );

    hs->cffd =  cga::Vec(Op::pj( kd.vec, Biv::xz)).norm();
    hs->lmod =  cga::Vec(Op::pj( kd.vec, Biv::xy)).norm();
    hs->lfreq = kd.writhe/6.0;
    hs->cdelay = .01 * kd.P / kd.Q;

    fm->lfreq = kd.energy / 100.;     /// Hovers around 1
    fm->c1 = 400 + std::fabs(kd.theta) * 40;
    fm->c2 = 400 + std::fabs(kd.phi) * 40;
  }

};


int main(int argc, char * argv[]) {
  if (argc > 1) {
    if (!strncmp(argv[1],"-n",2)){
      printf("Network Broadcast\n");
      ControlApp app("192.168.10.255");
      app.start();
    } else if (!strncmp(argv[1],"-l",2)){
      printf("%s \t", argv[1] );
      printf("Local Broadcast\n");
      ControlApp app("127.0.0.1");
      app.start();
    } else if (!strncmp(argv[1],"-r",2)){
      RenderApp<Knot> app;
      app.start();
    }
  } else {
    printf("please feed an argument:\n-n for network\n-l for local\n-r for render\n");
  }

   return 0;
}
