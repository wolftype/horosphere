/// the knot
#include "hsSimulator.hpp"
#include "hsRender.hpp"
#include "hsMacro.hpp"

#include "vsr/form/vsr_knot.h"
#include "vsr/form/vsr_shapes.h"

using namespace hs;
using namespace vsr;
using namespace vsr::cga;

struct Knot : UserBase {

  //Global Parameters to pass around
  struct Data  {

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

  } * mData;

  //PQ Function
  TorusKnot tk;

  //Meshes
  gfx::Mesh tm;             // Main Torus Knot Mesh
  gfx::Mesh tube;           // Tube Mesh

  vector<gfx::Mesh> wm;     // Some number of writhe Meshes;


  // Audio
  FMSynth * fm;
  WindSound * ws;
  Harmonics * hs;


  void onSetup(void * tsim){
    auto& s = *mData;
    auto& sim = simulator(this, tsim);

    fm = &sim.addAudioProcess<FMSynth>();
    ws = &sim.addAudioProcess<WindSound>();
    hs = &sim.addAudioProcess<Harmonics>();
    

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
    s.bDrawTube = true;
    s.bUseEnergies = true;

    s.energy_scale = 20;

    s.vec = vsr::cga::Vec(0,1,0);
    s.pnt = Round::null(3,0,0);
  }


  /*-----------------------------------------------------------------------------
   *  Update Local Data (called on all Apps)
   *-----------------------------------------------------------------------------*/
  void updateLocal(){
    auto& s = *mData;

    //Knot Settings
    tk.HF.vec() = s.vec;
    tk.P = s.P;
    tk.Q = s.Q;
    tk.amt = s.vel;

    //Knot Calculation
    tk.calc0(s.pnt);
    s.energy = tk.energy(0,100);

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
      float c = tk.energies[i] * s.energy_scale;
      c = CLAMP(c,0,1);
      float pt = TWOPI * (float)i/num;
      for (int j=0; j<res;++j){
        gfx::Vec4f tv( (1+sin(pt))/2.0,.2,(1+cos(pt))/2.0,.5);

        tube[i + j * num ].Col.set(c, 0, 1-c, .5);
        //Col.set(c*s.energy_scale*tv[0], tv[1], tv[2] + ( (1-c) * s.energy_scale ), tv[3]);
      }
    }

    //Writhe meshes
   for (int k = 0; k < s.numcables/2.; ++k){

       wm.push_back( gfx::Mesh(GL::LL) );
       wm.push_back( gfx::Mesh(GL::LL) );

       for(int i = 0; i < num; ++i){

        // double c = tk.energies[i] * s.energy_scale;
         double t = (float)i/num;

  //       if (s.bUseEnergies) t *= tk.energies[i] * s.energy_scale;

         Par tp = Round::pair( tk.cir[i].dilate( Round::loc(tk.cir[i]), 1.2 ), t * TWOPI * s.writhe);
         Pnt pa = Round::split( tp, true );
         Pnt pb = Round::split( tp, false );

         wm[k].add(  pa );
         wm[k+1].add( pb );
      }
   }

  }


  /*-----------------------------------------------------------------------------
   *  Update Global State (called from Control App)
   *-----------------------------------------------------------------------------*/
  void updateGlobal(){
    auto& s = *mData;

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
    auto& s = *mData;

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

    if (s.bDrawWrithe) {
      for(auto& i : wm ){
        i.drawVertices();
      }
    }

  }

  void onMessage(al::osc::Message& m){
      auto& s = *mData;
      auto ap = m.addressPattern();
      float tmp;
      if (ap=="/bAutoMode") {
        float t;
        m >> t;
        s.bAutoMode = (bool)t;
      } else if (ap == "/bFlow") {
        float t;
        m >> t;
        s.bFlow = (bool)t;
      } else if (ap == "/bDrawFibers") {
        float t;
        m >> t;
        s.bDrawFibers = (bool)t;
      } else if (ap == "/bDrawTube") {
        float t;
        m >> t;
        s.bDrawTube = (bool)t;
      } else if (ap == "/bDrawWrithe") {
        float t;
        m >> t;
        s.bDrawWrithe = (bool)t;
      } else if (ap == "/P") {
        m >> tmp;
        s.P = tmp;
      } else if (ap == "/Q") {
        m >> tmp;
        s.Q = tmp;
      } else if (ap == "/theta") {
        m >> tmp;
        s.theta = tmp;
      } else if (ap == "/phi") {
        m >> tmp;
        s.theta = tmp;
      } else if (ap == "/bUseEnergies"){
        float t;
        m >> t;
        s.bUseEnergies = (bool)t;
      } else if (ap == "/energy_scale"){
        float t;
        m >> t;
        s.energy_scale = t;
      } else if (ap == "/writhe"){
        float t;
        m >> t;
        s.writhe = t;
      }
  }

  void updateAudio(){
    auto& kd = *mData;
    
    auto diameter = Round::size( tk.cir[0], true );
    ws->freq = 330. + ( 110. * ( 10 * diameter ) );
    ws->width = 10 + ( 8000. * diameter );

    hs->cffd =  cga::Vec(Op::pj( kd.vec, Biv::xz)).norm();
    hs->lmod =  cga::Vec(Op::pj( kd.vec, Biv::xy)).norm();
    hs->lfreq = kd.writhe/6.0;
    hs->cdelay = .01 * kd.P / kd.Q;

    fm->lfreq = kd.energy / 100.;     /// Hovers around 1

    auto ttheta = kd.bAutoMode ? fabs((Op::pj(kd.vec, Biv::xz) <= cga::Vec::x)[0]) : kd.theta;
    auto tphi = kd.bAutoMode ? fabs((Op::pj(kd.vec, Biv::yz) <= cga::Vec::z)[0]) : kd.theta;
    fm->c1 = 40 + std::fabs(ttheta) * 40;
    fm->c2 = 40 + std::fabs(tphi) * 40;
  }

};

namespace hs{
///Named Boolean Parameters of Knot
template<> template<> void Param<bool>::specify(Knot::Data& k){
  mData = {
    {"bDrawFibers",&k.bDrawFibers},
    {"bDrawTube",&k.bDrawTube},
    {"bAutoMode",&k.bAutoMode},
    {"bDrawVec",&k.bDrawVec},
    {"bDrawPnt",&k.bDrawPnt},
    {"bDrawWrithe",&k.bDrawWrithe},
    {"bFlow",&k.bFlow},
    {"bUseEnergies", &k.bUseEnergies}
  };
}

template<> template<> void Param<float>::specify(Knot::Data& k){
  mData = {
    {"P",&k.P,1,10},
    {"Q",&k.Q,1,10},
    {"theta",&k.theta,1,10},
    {"phi",&k.phi,1,10},
    {"energy_scale", &k.energy_scale,0,100},
    {"writhe", &k.writhe,1,100}
  };
}
}

///MACRO
RUN(Knot);