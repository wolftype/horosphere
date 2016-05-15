/*
 * =====================================================================================
 *
 *       Filename:  xOhio.cpp
 *
 *    Description:  use of ohio event scheduling with gamma scheduling

                    some goals:
                        
                        explore LFOs (para, tri, up, impulse)
                        generate sounds, pass through delay
                        effects in the scheduler
                        
                        build control flow diagram with ohio 
                        for changing all of the above dynamically

                        visualize with versor
 *
 *        Version:  1.0
 *        Created:  04/23/2016 14:21:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


/// This executable must be run with one of the following flags:
/// -n: To Launch Simulator Broadcasting to AlloSphere network
/// -l: To Launch Simulator Broadcasting to Local network
/// -r: To Launch Renderer


#include "hsSimulator.hpp"
#include "hsRender.hpp"
#include "hsMacro.hpp"
#include "hsControl.hpp"
#include "hsObjects.hpp"

#include "ohio.hpp"

#include "vsr/draw/vsr_cyclide_draw.h"


using namespace hs;
using namespace vsr;
using namespace vsr::cga;


/*-----------------------------------------------------------------------------
 *  DATA
 *-----------------------------------------------------------------------------*/
/// User-defined Routines
struct User : UserBase {
  
  /// Global Parameters to pass around
  struct Data  {
    
    //Knot
    float f = 0.0;
    float p = 5;
    float q = 3;
    float scale = 1;
    float num = 100;
    float ecc = 1;

    //Crystal
    float cp = 3;
    float cq = 3;
    float bravaisType = 7;
    bool pbar, qbar, pqbar;
    float numX, numY, numZ;
    float crystalMotifMode;
    float cScale = .01;
    
    //Frame that Controls knot
    Frame knotFrame;
    // Knot Frame that orbits
    Frame frame = Frame(10,0,0);
    // Tangent to Fiber A at frame
    Pair tangentA;
    // Tangent to Fiber B at frame
    Pair tangentB;

    // Point that orbits
    Point point;
    // Whether to normalize
    bool bNormalize;

    
    // Diatom Motif
    Frame motif;


    // Particles in motion
    gfx::Vec3f particles[1000];

    // use camera
    bool bUseCam;

    // Draw booleans
    bool bDrawKnot = false;
    bool bDrawCrystal = true;

    
  } * mData;

  /// Behaviors
  map< std::string, ohio::behavior > mBehavior;
  
  /// Get behavior by name if it exists, create if it does not;
  ohio::behavior& behavior(const std::string& s) { 
    return mBehavior.emplace(s,ohio::behavior()).first -> second; 
  }  
  // Camera Frame
  Frame mCamera= Frame(0,0,5);

  //Some Settings
  bool bProcA;

  //Graphics 
  Diatom diatom;
  TorusKnot tk;
 // CircularNet net;
  Crystal crystal;

  Pair mTangent;
  
  //Audio Processes
  FMSynth * fm;
  Echo * echo;
  Voice * voice;
  SpectralNoise * spectralNoise;
  MagnitudeReader * magReader;

  Audio::ProcessNode * processA;            //<-- need to use ProcessNode type

  virtual void onSetup(void * s) ;
  virtual void onDraw() ;
  virtual void updateLocal() ;
  virtual void updateGlobal() ;
  virtual void updateAudio() ;
  virtual void onMessage(al::osc::Message& m) ;

  void setBehavior(int idx);
};

/// Move Camera to Target over n seconds
void move(User& user, const cga::Vec& target, float sec){
  
  float fps = 60.f;

  auto& b = user.behavior("move");

  auto vec  = target - user.mCamera.vec();
  auto dvec = vec.unit() * (vec.norm()/ (sec*fps));
  
  auto e1 = ohio::every_(1.f/fps, [&,dvec](float t){  user.mCamera.dx() = dvec; return true;} );
  auto e2 = [&,target](auto&& t){ return (cga::Vec(user.mCamera.pos()) - target).norm() < .001; }; 

  b.pollrateFinish = 1.f/fps;
  b.launch(e1).until( e2 );
}

/// Rotate Camera to Look at Target over n seconds
void look(User& user, const cga::Vec& target, float sec){
  
  auto& b = user.behavior("look");

  float fps = 60.f;
  int numSteps = sec * fps;
  //function of amt t [0-1) 
  auto go_func =[&,numSteps](float t){ 
    auto amt = 1.0/((1-t)*numSteps);
    auto biv = user.mCamera.relOrientBiv(target, false);
    user.mCamera.db() = biv * amt;
    return true;
  };
  
  auto end_func = [=,&user](auto&& t){ 
    auto tmp = (target - user.mCamera.vec()).unit(); 
    float f = ((-user.mCamera.z()) <= tmp)[0];
    return f > .9999; 
  }; 

  auto sigfun = ohio::map_(sec, go_func);
  auto e1 = ohio::every_(1.f/fps, sigfun);

  b.pollrateFinish = 1.f/fps;
  b.launch(e1).until( end_func );
  
}


/*-----------------------------------------------------------------------------
 *  REACTIONS to commands
 *-----------------------------------------------------------------------------*/
namespace hs {


  template<> void behave(User& user){
    //one move over 1 second =  
    look(user, cga::Vec(0,1,0), 3 );
    move(user, cga::Vec(2,0,0), 3 );

  }

  
}

  

  
/*-----------------------------------------------------------------------------
 *  SETUP
 *-----------------------------------------------------------------------------*/
  inline void User::onSetup(void * s)  {

    ohio::AppStartTime = ohio::now();

    ///0. Get Reference to Simulator
    auto& sim = simulator(this,s);
    
    ///1. Add processes to sim (may automagically make gui for it)
    processA = &sim.audio.mScheduler.add<Audio::ProcessNode>();//processA);  
    
    magReader = &sim.addAudioProcess<MagnitudeReader>(*processA);
    echo = &sim.addAudioProcess<Echo>(*processA);
//    spectralNoise = &sim.addAudioProcess<SpectralNoise>(*processA);
    voice = &sim.addAudioProcess<Voice>(*processA); 

      
    ///2. Start Polling for Command Line Input Events
    ///               callback func      pollrate     event stream
    ohio::callback2_( hs::userCB(*this), .5 )( ohio::listener_( ohio::stdin_ ) );

    ///3. Schedule Events and spawn time-based listener  
    setBehavior(1);
   // setBehavior(1);
      
  }

  inline void User::setBehavior(int idx){

   auto& s = *mData;
    
   auto& b1 = behavior("pulse");
   auto& b2 = behavior("hit");

   //Grow Diatom Diamter
   auto grow = [this](auto&& t){
     mData -> motif.dx() += cga::Vec::x*magReader->max; 
     return true;
   };

   //Shrink Diatom Diameter
   auto shrink = [this](auto&& t){
     mData -> motif.dx() += (mData->motif.vec()).unit() * -.05;
     return true;
   };

   //Pulse Motif Rotation
   auto pulse = [this](auto&& t){
     mData -> motif.db() = Biv( Rand::Num() * .05, Rand::Num() *.05, Rand::Num() *.05 );
     return true;
   };
   
   //Make a Sound
   auto beep = [this](auto&& setfreq){ //also set location?
     return [=](auto&& t){
       voice->freq = 120 + 1140 * setfreq();
       voice->env.reset();
       return true;
     };
   };  

   //Nudge Fiber 
   auto nudgeFiber = [this](bool A){
     
      return[=](auto&& x){
        auto vec1 =(-Round::direction( A ? mData->tangentA : mData->tangentB ).copy<cga::Vec>().unit());
        auto vec2 = -mData->frame.vec().unit();
        auto biv = vec1^vec2;//cga::Vec(Round::carrier( tk.HF.fiberA() ).dual()).unit() ;
        mData -> knotFrame.db() = biv * .03 + Biv( Rand::Num() * .005, Rand::Num() *.005, Rand::Num() *.005 );
        return true;
      };
   };

   auto bCross = [this](bool A ){
     return[=](){
      return (mData->frame.pos() <= Round::carrier( A ? tk.HF.fiberA() : tk.HF.fiberB()).dual() )[0] > 0;
     };
   };
   
   auto fCross = [this]( bool A ){
     return[=](){
      auto t = Round::theta( A ? tk.HF.fiberA() : tk.HF.fiberB(), mData->frame.pos());
      return t/PI;
     };
   };
   
   switch(idx){
     case 0:
     {
       auto e1 = ohio::tag2_( ohio::triggerval_(magReader->bTrigger), grow );
       auto e2 = ohio::tag2_( ohio::triggerval_(magReader->bTrigger), pulse );
       auto e3 = ohio::tag2_( ohio::triggerval_(!magReader->bTrigger), shrink );
       b1.launch( e1, e2, e3);
       break;
     }
     case 1:
     {
       auto e1 = ohio::tag2_( ohio::trigger_( bCross(true) ), beep( fCross(true) ) ); 
       auto e2 = ohio::tag2_( ohio::trigger_( bCross(false) ), beep( fCross(false) ) ); 
       b2.launch( e1, e2 );
       break;
     }
     case 2:
     {
       auto e1 = ohio::tag2_( ohio::trigger_( bCross(true) ),  hana::split_( beep( fCross(true) ), nudgeFiber(true)) );
       auto e2 = ohio::tag2_( ohio::trigger_( bCross(false) ),  hana::split_( beep( fCross(false) ), nudgeFiber(false)) );
      // auto e2 = ohio::tag2_( ohio::trigger_( bCrossA),  nudgeFiber );
     //  auto e2 = ohio::tag2_( ohio::trigger_( bCrossA),  nudgeFiber );
     //  auto e2 = ohio::tag2_( ohio::trigger_( bCrossA), nudgeFiber );

       b2.launch( e1,e2 ); 
     }
   }
    

  }



/*-----------------------------------------------------------------------------
 *  DRAW
 *-----------------------------------------------------------------------------*/
  /// Render to Screen (called six times)
  inline void User::onDraw()  {
    auto& s = *mData;

  //  Draw(diatom);

   // Draw(mCamera); 
    
    //Draw Knot
    if (s.bDrawKnot){
      Draw( tk.HF.fiberA());  
      Draw( tk.HF.fiberB()); 
      Draw( mTangent, 1,0,0 );
  
      glPointSize(5);
      for (auto& i : tk.pnt ){
        Draw(i,0,1,1);
      }

      for (auto& i : tk.cir ){
        Draw(i,0,1,0);
      }
    }

    
    //Draw Crystal Group
    
    if (s.bDrawCrystal){
      Draw( crystal );
     // for (auto& i : crystal.sg.hang( crystal.sg.apply( s.frame.pos() ), 1, 1, 1 ) ) Draw(i); 
    }
  }

  
/*-----------------------------------------------------------------------------
 *  Update
 *-----------------------------------------------------------------------------*/
  /// Update Local Parameters on each Computer
  inline void User::updateLocal()  {
    auto& s = *mData;

    //knot data
    tk.set(s.p,s.q);
    s.knotFrame.scale( s.scale );
    tk.HF.vec() = s.knotFrame.y();
    tk.HF.cir() = s.knotFrame.cxz();
    auto tp = s.frame.pos(); tp[3] = s.ecc;
    tk.apply( tp, s.num, s.bNormalize);

    
    mTangent = Pair(-Round::direction( tk.cir[0] ).dual().copy<Tnv>()).translate( s.frame.pos() );

    //diatom data
    diatom.motif() = s.motif;


    //crystal spacegroup data
     cga::Vec ratioVec(1,1,1);//s.xratio, s.yratio, s.zratio); 
     
     // bravais type and lattice type settings -- lattice set to primitive
     SpaceGroup3D<cga::Vec>::Lattice lattice = {(int)s.bravaisType, 1};//(int)s.latticeType};
     SpaceGroup3D<cga::Vec>::Glide glide;// = { {(int)s.glideA, s.bGlideA},{(int)s.glideB,s.bGlideB},{(int)s.glideC,s.bGlideC} };
     SpaceGroup3D<cga::Vec>::Screw screw;// = { (int)s.screwAB, (int)s.screwBC, (int)s.screwAC, (int)s.screwAB_trs, (int)s.screwBC_trs };
    
     crystal.sg.set( s.cp, s.cq, s.pbar, s.qbar, s.pqbar, lattice, ratioVec, glide, screw);
     crystal.mFrame = s.frame;
     crystal.mFrame.scale( s.cScale );
     crystal.mNumX = s.numX; crystal.mNumY = s.numY; crystal.mNumZ = s.numZ;
     crystal.mMode = s.crystalMotifMode;
     crystal.apply();
    


  }
  
  /// Update Global Parameters (::Data) to share
  inline void User::updateGlobal()  {
    
    auto& s = *mData;
    

    // Move frame around knot orbit
    auto par = tk.dpar( (int)s.f + 1);
    auto bst = Gen::bst(par); 
    s.frame.boost(bst);

    //Tangent at frame in direction along orbit
    s.tangentA = Round::renormalize( Tangent::at( s.frame.pos() ^ tk.HF.fiberA().dual() , s.frame.pos() ) );
    s.tangentB = Round::renormalize( Tangent::at( s.frame.pos() ^ tk.HF.fiberB().dual() , s.frame.pos() ) );


    //Knot step();
    s.knotFrame.step();

    //Diatom motif
    s.motif.step();

    //Virtual Camera
    mCamera.step();

    // use virtual camera?
    if (s.bUseCam) mApp->scene.camera.set( mCamera.pos(), mCamera.quat() ); 

    
  }
  
  /// Update Audio Parameters (e.g. based on geometry)
  inline void User::updateAudio()  {}
  /// Receive OSC Messages
  inline void User::onMessage(al::osc::Message& m)  {}



/*-----------------------------------------------------------------------------
 *  GUI PARAMETERS
 *-----------------------------------------------------------------------------*/
/// User Defined GUI Specifications
template<> template<>
void Param<float>::specify(User::Data& k){
  //---------label, dataPtr, min,max
  mData = {   
    //Knot
    {"f", &(k.f),0,1000},
    {"p", &(k.p),0,10},
    {"q", &(k.q),0,10},
    {"scale", &(k.scale),1,10},
    {"num",&(k.num),0,1000},
    {"ecc",&(k.ecc),-10,10},
    
    //Diatom ...

    //Crystal
    {"cp",&(k.cp),2,6},
    {"cq",&(k.cq),2,6},
    {"bravais",&(k.bravaisType),0,7},
    {"numX",&(k.numX),1,20},
    {"numY",&(k.numY),1,20},
    {"numZ",&(k.numZ),1,20},
    {"cMode",&(k.crystalMotifMode),0,10},
    {"cScale",&(k.cScale),0,10}
  };
}

template<> template<>
void Param<bool>::specify(User::Data& k){
  //---------label, dataPtr, min,max
  mData = {   
    {"bUseCam", &(k.bUseCam)},
    {"bNormalize",&(k.bNormalize)},

    //Crystal
    {"pbar",&(k.pbar)},
    {"qbar",&(k.qbar)},
    {"pqbar",&(k.pqbar)}
  };
}

//template<> template<>
//void Param<float>::specify(Crystal& k){
//
//}
//
//template<> template<>
//void Param<float>::specify(Crystal& k){
//
//  mData = {
//    {"abar",&(k.sg.mBar.a)},
//    {"bbar",&(k.sg.mBar.b)},
//    {"abbar",&(k.sg.mBar.ab)}
//  };
//}

/// Run Simulation or Rendering Program
RUN(User)
