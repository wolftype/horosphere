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
#include "hsObjects.hpp"

#include "vsr/draw/vsr_cyclide_draw.h"
#include "vsr/form/vsr_shapes.h"

#ifndef __lubuntu__
#include "hsControl.hpp"
#include "ohio.hpp"
#endif

using namespace hs;
using namespace vsr;
using namespace vsr::cga;

#define NUMPARTICLES 1024

/*-----------------------------------------------------------------------------
 *  DATA
 *-----------------------------------------------------------------------------*/
/// User-defined Routines
struct User : UserBase {
  
  /// Global Parameters to pass around
  struct Data  {
    
    //Knot
    float frame_orbit_speed = 0.001;
    float frame_orbit_max_speed = .1;
    float particle_orbit_speed = .001;
    float p = 5;
    float q = 3;
    float scale = 1;
    float num = 100;
    float ecc = 1;

    //Crystal
    float cp = 3;
    float cq = 3;
    float bravaisType = 7;
    float diatom_force = .1;
    bool pbar, qbar, pqbar;
    float numX, numY, numZ;
    float crystalMotifMode;
    float cScale = 0;
    float xratio = 1;
    float yratio = 1;
    float zratio = 1;

    float linewidth =1;
    float pointsize =5;
    float tube_opacity = 1;
    
    //Frame that Controls knot fibers
    Frame knotFrame;
    // Knot Frame that orbits
    Frame frame = Frame(10,0,0);
    // offset along y of knot orbit
    float yoffset = 0.5;
    // Crystal Frame
    Frame crystalFrame = Frame(2,0,0);
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


    // Particles in motion to be tied to Spectral Noise
   // struct Trace { vector<Point> particles = vector<Point>(5); };
   Point particles[NUMPARTICLES];
   float magbin[NUMPARTICLES];
   float phasebin[NUMPARTICLES];

   float mNumTrace = 5;
   float amp = 2;

    // use camera
    bool bUseCam =false;

    // Draw booleans
    bool bDrawKnot = false;
    bool bDrawCrystal = true;
    bool bDrawParticles = false;
    bool bDrawDiatom = false;
    bool bDrawTube = false;
    bool bDrawFibers = false;
    bool bDrawTangent = false;
    bool bDrawCircles = false;
    bool bReset = false;

    
  } * mData;

  #ifndef __lubuntu__
  /// Behaviors
  map< std::string, ohio::behavior > mBehavior;
  /// Get behavior by name if it exists, create if it does not;
  ohio::behavior& behavior(const std::string& s) { 
    return mBehavior.emplace(s,ohio::behavior()).first -> second; 
  }
  #endif 
  
  // Camera Frame
  Frame mCamera= Frame(0,0,5);

  //Some Settings
  bool bProcA;

  //Graphics 
  Diatom diatom;
  TorusKnot tk;
  gfx::Mesh tube;

 // CircularNet net;
  Crystal crystal;
  bool bCrystalHit = false;
  
  Pair mTangent;
  vector<Point> trace;
  Point mParticles[NUMPARTICLES];
  
  //Audio Processes
  FMSynth * fmsynth;
  WindSound * wind;
  Echo * echo;
  SpectralInfo * spectralInfo;
  Voice * voiceA;
  Voice * voiceB;
  SpectralNoise * spectralNoise;
  MagnitudeReader * magReader;

  Audio::ProcessNode * processA;            //<-- need to use ProcessNode type

  virtual void onSetup(void * s) ;
  virtual void onDraw() ;
  virtual void updateLocal() ;
  virtual void updateGlobal() ;
  virtual void updateAudio() ;
  virtual void onMessage(al::osc::Message& m) ;
  bool setBehavior(int idx);


  void reset();
};

void User::reset(){
  auto& s = *mData;
  
  Field<Point> field(8,8,16,1);
  
  for (int i=0;i<field.num();++i){
    s.particles[i] = field.grid(i).translate( Rand::Num(), Rand::Num(), Rand::Num() );
    s.particles[i][3] = s.ecc;
  }

  s.motif = Frame();
  s.frame = Frame(10,0,0);

};

///// Move Camera to Target over n seconds
//void move(User& user, const cga::Vec& target, float sec){
//  
//  float fps = 60.f;
//
//  auto& b = user.behavior("move");
//
//  auto vec  = target - user.mCamera.vec();
//  auto dvec = vec.unit() * (vec.norm()/ (sec*fps));
//  
//  auto e1 = ohio::every_(1.f/fps, [&,dvec](float t){  user.mCamera.dx() = dvec; return true;} );
//  auto e2 = [&,target](auto&& t){ return (cga::Vec(user.mCamera.pos()) - target).norm() < .001; }; 
//
//  b.pollrateFinish = 1.f/fps;
//  b.launch(e1).until( e2 );
//}
//
///// Rotate Camera to Look at Target over n seconds
//void look(User& user, const cga::Vec& target, float sec){
//  
//  auto& b = user.behavior("look");
//
//  float fps = 60.f;
//  int numSteps = sec * fps;
//  //function of amt t [0-1) 
//  auto go_func =[&,numSteps](float t){ 
//    auto amt = 1.0/((1-t)*numSteps);
//    auto biv = user.mCamera.relOrientBiv(target, false);
//    user.mCamera.db() = biv * amt;
//    return true;
//  };
//  
//  auto end_func = [=,&user](auto&& t){ 
//    auto tmp = (target - user.mCamera.vec()).unit(); 
//    float f = ((-user.mCamera.z()) <= tmp)[0];
//    return f > .9999; 
//  }; 
//
//  auto sigfun = ohio::over_(sec, go_func);
//  auto e1 = ohio::every_(1.f/fps, sigfun);
//
//  b.pollrateFinish = 1.f/fps;
//  b.launch(e1).until( end_func );
//  
//}


/*-----------------------------------------------------------------------------
 *  REACTIONS to commands
 *-----------------------------------------------------------------------------*/
namespace hs {

  #ifndef __lubuntu__
  template<> void behave(User& user, int idx){
    user.setBehavior(idx);
  }
  #endif

  
}

  

  
/*-----------------------------------------------------------------------------
 *  SETUP
 *-----------------------------------------------------------------------------*/
  inline void User::onSetup(void * s)  {


    ///0. Get Reference to Simulator
    auto& sim = simulator(this,s);
    
    ///1. Add processes to sim (may automagically make gui for it)
    processA = &sim.audio.mScheduler.add<Audio::ProcessNode>();//processA);  
    
    magReader = &sim.addAudioProcess<MagnitudeReader>(*processA);
    spectralInfo = &sim.addAudioProcess<SpectralInfo>(*processA);
    echo = &sim.addAudioProcess<Echo>(*processA);
    spectralNoise = &sim.addAudioProcess<SpectralNoise>(*processA);
    voiceA = &sim.addAudioProcess<Voice>(*processA); 
    voiceB = &sim.addAudioProcess<Voice>(*processA); 
    fmsynth = &sim.addAudioProcess<FMSynth>(*processA);

    /// Initialize all variables
    reset();
 
    #ifndef __lubuntu__  
    ///2. Start Polling for Command Line Input Events
    ohio::AppStartTime = ohio::now();
    ///               callback func      pollrate     event stream
    ohio::callback2_( hs::userCB(*this), .5 )( ohio::listener_( ohio::stdin_ ) );

   
    ///3. Schedule Events and spawn time-based listener  
    //COMPOSITION
    
    auto first =  [this](auto&& t) { cout << "hello" << endl; this -> setBehavior(1); return true; };
    auto second = [this](auto&& t) { setBehavior(2); return true; };
    auto third = [this](auto&& t) { setBehavior(3); return true; };

    auto e0 = ohio::tag2_( ohio::at_(1, true), first );
    auto e1 = ohio::tag2_( ohio::at_(60,true), second );
    auto e2 = ohio::tag2_( ohio::at_(120,true), third );

    auto& b = behavior("composition");
   // b.launch(e0,e1);
   // setBehavior(1);
      
    #endif 
  }

  inline bool User::setBehavior(int idx){
   
   #ifndef __lubuntu__
   auto& s = *mData;
    
   auto& b1 = behavior("pulse");
   //auto& b2 = behavior("hit");
  // auto& b3 = behavior("crystalHit");

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
   
   auto pentatonic = [](float t){
      double hz[5] = { scl::freq("b-2"), scl::freq("d-2"), scl::freq("e-2"), scl::freq("g-2"), scl::freq("a-2") };
      return hz[(int)(scl::clipMag(t) * 5)];
   };

   //Make a Sound
   auto beep = [this](auto&& setfreq, bool b){ //also set location?
     double hz[6] = { scl::freq("b-1"), scl::freq("d-2"), scl::freq("e-3"), scl::freq("g45"), scl::freq("a-5"), scl::freq("b-6") };
     return [=](auto&& t){
       auto idx = (int)(setfreq() * 5);
       if (idx < 0 || idx > 5) idx = 0;
       cout << hz[idx] << endl;
       if (b){
        voiceA->freq = hz[idx];//pentatonic( setfreq() );
        voiceA->env.reset();
       } else {
        voiceB->freq = hz[idx];
        voiceB->env.reset();
       }
       return true;
     };
   };  

   //Nudge Fiber 
   auto nudgeFiber = [this](bool A){
     
      return[=](auto&& x){
        auto vec1 =(-Round::direction( A ? mData->tangentA : mData->tangentB ).copy<cga::Vec>().unit());
        auto vec2 = -mData->frame.vec().unit();
        auto biv = vec1^vec2;//cga::Vec(Round::carrier( tk.HF.fiberA() ).dual()).unit() ;
        mData -> knotFrame.db() = biv * .003 + Biv( Rand::Num() * .005, Rand::Num() *.005, Rand::Num() *.005 );
        return true;
      };
   };

   auto bCross = [this](bool A ) {
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

  auto genRandomNumber = [](float beg, float range ){ 
    return[=](){ return  beg + Rand::Num() * range; }; 
  };

  auto makeFreqFromCrystal = [genRandomNumber,this]( bool b){
    return [genRandomNumber,this,b](){ return genRandomNumber( .2 + (b ? 1.0/mData->cp : 1.0/mData->cq), .2)(); };
  };

  //auto  
  
   /// Launch Behaviors 
   switch(idx){
     case 0:
     {
       s.bDrawDiatom = true;
       auto e1 = ohio::tag2_( ohio::triggerval_(magReader->bTrigger), grow );
       auto e2 = ohio::tag2_( ohio::triggerval_(magReader->bTrigger), pulse );
       auto e3 = ohio::tag2_( ohio::triggerval_(!magReader->bTrigger), shrink );
       b1.launch( e1, e2, e3);
       break;
     }
     case 1: //knot orbit
     {
       reset();

       voiceA -> attack = 0.061; voiceB -> attack = 0.041;   
       voiceA -> decay = 0.041; voiceB -> decay = 0.061;       
       voiceA -> mode = Voice::PULSE; voiceB -> mode = Voice::PULSE;       
       s.bDrawCrystal=false;
       s.bDrawKnot = true; 
       s.bDrawParticles = false;
       s.bUseCam = true; 
       mCamera.pos() = PAO;
       s.scale = 7;
       s.num = 400;
       s.linewidth = 30; s.pointsize = 30;
       
       auto e1 = ohio::tag2_( ohio::trigger_( bCross(true) ), beep( fCross(true),true ) ); 
       auto e2 = ohio::tag2_( ohio::trigger_( bCross(false) ), beep( fCross(false),false) ); 
       auto e3 = ohio::every_(.1, ohio::over_(60, [this](float t ){ mData->frame_orbit_speed = .0001 + t * mData->frame_orbit_max_speed; return true;} ) );
       auto e4 = ohio::every_(.1, ohio::over_(60, [this](float t ){ voiceA -> attack = .0031 + t * .01; return true;} ) );
       auto e45 = ohio::every_(.1, ohio::over_(60, [this](float t ){  voiceB -> attack = .0031 + t * .01; return true;} ) );

       auto e5 = ohio::every_(.1, ohio::over_(60, [this](float t ){ mData->p = t * 3; return true;} ) );
       auto e55 = ohio::every_(.1, ohio::over_(60, [this](float t ){ mData->q = 5 + t * 30; return true;} ) );

       auto e6 = ohio::every_(.5, [this](auto&& t){ voiceA -> mode = (int)Rand::Num(4); return true; } );
       auto e65 = ohio::every_(.4, [this](auto&& t){ voiceB -> mode = (int)Rand::Num(4); return true; } );

       auto e7 = ohio::every_(.1, ohio::over_(60, [this](float t ){ echo->delayMax = .001 + t * .30; return true;} ) );
       auto e8 = ohio::every_(.1, [this](auto&& t){ 
          auto p = mData -> frame.vec();
          voiceA -> src.pos = Vec3f( p[0], p[1], p[2] ); 
          return true;} ); 
       b1.launch( e1, e2, e3, e4, e45, e5, e55, e6, e65, e7 );
       break;
     }
     case 2: //Hopf Fibers Nudge

     {
       voiceA -> attack = 0.031; voiceB -> attack = 0.031;   
       voiceA -> decay = 0.031; voiceB -> decay = 0.031; 
       echo -> delayMax = .0001;      
       voiceA -> mode = Voice::PULSE; voiceB -> mode = 5;
       s.bDrawCrystal=false;
       s.bDrawKnot = true; 
       s.bDrawParticles = false;
       auto e1 = ohio::tag2_( ohio::trigger_( bCross(true) ),  hana::split_( beep( fCross(true), true ), nudgeFiber(true)) );
       auto e2 = ohio::tag2_( ohio::trigger_( bCross(false) ),  hana::split_( beep( fCross(false),false ), nudgeFiber(false)) );
       auto e3 = ohio::every_( .1, ohio::over_(30, [this](float t ){ mData->frame_orbit_speed = .001 + t * mData->frame_orbit_max_speed; return true;} ) );
       auto e4 = ohio::every_( .1, ohio::over_(15, [this](float t){ fmsynth -> mMix = .03 * t; return true; } ));
     //  auto e5 = ohio::tag2_( ohio::trigger_( bCross(true) ),
 //      auto e2 = ohio::tag2_( ohio::trigger_( bCross(false) ),  hana::split_( beep( fCross(false),false ), nudgeFiber(false)) );

       b1.launch( e1, e2, e3, e4 ); 
       break;
     }
     case 3: //Particles     
     {
        s.bUseCam = true; 
        echo -> delayMax = .7;  
        s.bDrawParticles = true; s.amp = 1000;
        spectralNoise -> mMix = .5;
        auto e1 = ohio::every_(.1, ohio::over_(10, [this](float t){ spectralNoise->max = 100 + 1000 * t; return true; } ));
        auto e2 = ohio::every_(.1, ohio::over_(20, [this](float t){ spectralNoise->thresh = .01 * t; return true; }));
        auto e3 = ohio::every_(.1, ohio::over_(30, [this](float t){ spectralNoise->min = 40 + 300 * t; return true; } ));
        auto e4 = ohio::every_(.1, [this](auto&& t){ 
            auto v = mData->frame.vec(); 
            spectralNoise -> src.pos = gfx::Vec3f(v[0],v[1],v[2]);
            return true; } );
        b1.launch( e1, e2, e3, e4 );//.over(10);//.until( [this](){ }; );
        break;
     }
      case 4: //Inside Tube
     {
        s.bUseCam = true; 
        s.p = 6; s.q = 1;
        s.frame_orbit_speed = .0001;
        s.num = 400;
        s.bDrawTube = true;
        s.tube_opacity = .9;
        s.yoffset = 3;
        s.particle_orbit_speed = .00021;
        auto e1 = ohio::every_(.1, [this](auto&& t){ mCamera =  mData->frame.moveY( mData->yoffset ); return true; });
        auto e2 = ohio::every_(.1, ohio::over_(120, [this](float t){ mData -> q = (int)(t * 7); return true; }));

        //.relTwist( mData->frame.moveY( mData->yoffset ), .2 ); return true; } );

        b1.launch(e1, e2);
        break;
     }
     case 5:  /// Crystal Symmetries
     {     
       //Settings
       s.bDrawDiatom = false;
       s.bDrawKnot = false;
       s.bDrawCrystal = true;
       s.bDrawParticles = true;
       s.linewidth = 2;
       s.q =0; s.cScale = .1; 
       voiceA -> attack = 0.001; voiceB -> attack = 0.001;        
       voiceA -> mode = Voice::PULSE; voiceB -> mode = Voice::TRI;
       s.crystalMotifMode = 2;
       s.crystalFrame.scale() = .01;
       s.bUseCam = true; 
       echo -> delayMax = .3;
       mCamera.pos() = PAO;
       s.frame_orbit_max_speed = .002;

       //Behaviors
       auto e1 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), beep( makeFreqFromCrystal(true), true ) ); 
       auto e2 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), beep( makeFreqFromCrystal(false), false ) ); 
       auto e3 = ohio::every_(.1, ohio::over_(60, [this](float t){ mData -> q = t * 2; return true; } ) );
       auto e4 = ohio::every_(.1, ohio::over_(5, [this](float t){ voiceA -> attack = .001 + (1-t) * .03; return true; } ) );
       auto e45 = ohio::every_(.1, ohio::over_(35, [this](float t){ voiceB -> attack = .001 + (1-t) * .03; return true; } ) );
       auto e5 = ohio::every_(.1, ohio::over_(30, [this](float t){ mData -> frame_orbit_speed = .00001 + mData -> frame_orbit_max_speed * t; return true; } ) );
       auto e6 = ohio::after_(10, [this](auto&& t){ mData -> cp = 4; return true; } );
       auto e7 = ohio::after_(30, [this](auto&& t){ mData -> numX = 2; mData -> numY = 2; mData -> numZ = 2;return true; } ); 
       auto e8 = ohio::after_(60, [this](auto&& t){ mData -> numX = 4; mData -> numY = 4; mData -> numZ = 4;return true; } );
       auto e9 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), [this](auto&& t){ mData -> cScale = .1 * ( Rand::Boolean() ? -1 : 1); return true; } );

       b1.launch( e1, e2, e3, e4, e45, e5, e6, e7, e8, e9 );
       break;
     }    
     case 6: //tests
     {

       auto e1 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), beep( makeFreqFromCrystal(true), true ) ); 
       auto e2 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), beep( makeFreqFromCrystal(false), false ) ); 
       auto e3 = ohio::tag2_( ohio::trigger_( bCross(true) ), beep( fCross(true),true ) ); 
       auto e4 = ohio::tag2_( ohio::trigger_( bCross(false) ), beep( fCross(false),false) ); 

       b1.launch( e1, e2, e3, e4 );
       break; 
     }
     case 7: //tests
     {

       auto e1 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), beep( makeFreqFromCrystal(true), true ) ); 
       auto e2 = ohio::tag2_( ohio::triggerval_( bCrystalHit ), beep( makeFreqFromCrystal(false), false ) ); 
       auto e3 = ohio::tag2_( ohio::trigger_( bCross(true) ),  hana::split_( beep( fCross(true), true ), nudgeFiber(true)) );
       auto e4 = ohio::tag2_( ohio::trigger_( bCross(false) ),  hana::split_( beep( fCross(false),false ), nudgeFiber(false)) );

       b1.launch( e1, e2, e3, e4 );
       break; 
     }       
   }
   #endif
   
   return true;
    
  }


/*-----------------------------------------------------------------------------
 *  DRAW
 *-----------------------------------------------------------------------------*/
  /// Render to Screen (called six times)
  inline void User::onDraw()  {
    auto& s = *mData;

      //Per Frame Settings
     glPointSize(s.pointsize);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     glLineWidth(s.linewidth);   

    if (s.bDrawDiatom){
      Draw(diatom);
    }
    
    //Draw Knot
    if (s.bDrawKnot){

      if (s.bDrawFibers){
        Draw( tk.HF.fiberA());  
        Draw( tk.HF.fiberB()); 
      }
      if (s.bDrawTangent){
        Draw( mTangent, 1,0,0 );
      }
      if (s.bDrawTube){
        tube.drawElementsColor();
      }
      if (s.bDrawCircles) {
        for (auto& i : tk.pnt ){
          Draw(i,0,1,1);
        } 
        for (int i=0;i<tk.cir.size()-1;++i ){
          float t = (float)i/tk.cir.size();
          Draw(tk.cir[i],0,1,t);
         for (int j=0;j<3;++j){
           float tt = (float)j/3;
           Draw( Round::point( tk.cir[i], tt*PI ), tt , 0, 1-tt );
         }

        }
      }
    }

    if (s.bDrawParticles){
      glBegin(GL_POINTS);
      int iter =0;
      for (int i=0; i<NUMPARTICLES;++i){
        iter++; float t = (float)iter/NUMPARTICLES;
       // Draw(  mParticles[i],t,fabs(s.phasebin[i]),1-t, fabs(s.magbin[i]) * s.amp);
        glColor4f( t,fabs(s.phasebin[i]), 1-t, fabs(s.magbin[i]) * s.amp);
        glVertex3f( mParticles[i][0], mParticles[i][1],mParticles[i][2]);
      }
      iter =0;
      for (int i=0; i<NUMPARTICLES;++i){
        for (int j =0; j<s.mNumTrace;++j){
         float t = (float)iter/trace.size();        
        // Draw( trace[iter],.6*t,fabs(s.phasebin[i]),.6 *(1-t), fabs(s.magbin[i]) * s.amp);
         glColor4f( .6*t, fabs(s.phasebin[i]), .6 *(1-t), fabs(s.magbin[i]) * s.amp);
         glVertex3f( trace[iter][0], trace[iter][1],trace[iter][2]);
         iter++;
        }
      }
      glEnd();
    }

    //Draw Crystal Group  
    if (s.bDrawCrystal){
      Draw( crystal );
    }
  }

  
/*-----------------------------------------------------------------------------
 *  Update
 *-----------------------------------------------------------------------------*/
  /// Update Local Parameters on each Computer
  inline void User::updateLocal()  {
    auto& s = *mData;

    // set knot data
    tk.set(s.p,s.q);

    tk.HF.vec() = s.knotFrame.y();
    tk.HF.cir() = s.knotFrame.cxz();
    
    // apply to frame position
    auto tp = s.frame.pos(); tp[3] = s.ecc;
  //  cout << tp << endl;

    if (s.bDrawKnot){ 
      tk.apply( tp, s.num, s.bNormalize);
     // tk.calc0( tp );
      // create tube
      tube.clear(); 
      tube = Shape::Skin( tk.cir, tk.cir.size()-2, false, 5 );
      for (auto& i : tube.vertex()){
        i.Col[3] = s.tube_opacity;
      }
      // determine tangent at location in orbit 
      mTangent = Pair(-Round::direction( tk.cir[0] ).dual().copy<Tnv>()).translate( s.frame.pos() );
    }
    
    //Flow trace
    if (s.bDrawParticles){
       trace = vector<Point>( (int)s.mNumTrace * NUMPARTICLES);
       auto bst = tk.dbst( (int)(1.0/s.particle_orbit_speed) );
       for (int i =0; i<NUMPARTICLES; i++){
         trace[i*s.mNumTrace] = s.particles[i].spin(bst);
         for (int j=1;j<(int)s.mNumTrace;++j){
          int idx = i*(int)s.mNumTrace +j;
          trace[idx] = trace[idx-1].spin(bst);
          trace[idx-1] = Round::loc(trace[idx-1]);
         }
         int last_idx = i*(int)s.mNumTrace+(int)s.mNumTrace-1;
         trace[last_idx] = Round::loc(trace[last_idx]);
         mParticles[i] = Round::loc(s.particles[i]); //normalize
       };
    }

    //diatom data
    if (s.bDrawDiatom){
      diatom.motif() = s.motif;
      diatom.set(s.cp, s.cq);
      diatom.mForce = s.diatom_force;
      diatom.build();
    }
    
    if (s.bDrawCrystal){
     //crystal spacegroup data
     cga::Vec ratioVec(s.xratio, s.yratio, s.zratio); 
     
     // bravais type and lattice type settings -- lattice set to primitive
     SpaceGroup3D<cga::Vec>::Lattice lattice = {(int)s.bravaisType, 1};//(int)s.latticeType};
     SpaceGroup3D<cga::Vec>::Glide glide;// = { {(int)s.glideA, s.bGlideA},{(int)s.glideB,s.bGlideB},{(int)s.glideC,s.bGlideC} };
     SpaceGroup3D<cga::Vec>::Screw screw;// = { (int)s.screwAB, (int)s.screwBC, (int)s.screwAC, (int)s.screwAB_trs, (int)s.screwBC_trs };
    
     crystal.sg.set( s.cp, s.cq, s.pbar, s.qbar, s.pqbar, lattice, ratioVec, glide, screw);
     crystal.mFrame = s.crystalFrame;
     //crystal.mFrame.scale( s.cScale );
     crystal.mNumX = s.numX; crystal.mNumY = s.numY; crystal.mNumZ = s.numZ;
     crystal.mMode = s.crystalMotifMode;
     crystal.apply();
  
 
    }


  }
  
  /// Update Global Parameters (::Data) to share
  inline void User::updateGlobal()  {
    
    auto& s = *mData;

    if (s.bReset) { reset(); s.bReset = false; }
    

    // Move frame around knot orbit (maybe look up mTangent instead?)
    auto par = tk.dpar( (int)(10.0/(s.frame_orbit_speed)) );
    auto bst = Gen::bst(par); 
    auto tp = s.frame.pos(); tp[3] = s.ecc;
    auto p1 = tp.spin(bst);
    auto p2 = p1.spin(bst);
    p1 = Round::loc(p1); p2 = Round::loc(p2); //renormalize 
    s.frame.pos() = p1;  s.frame.orient( p2 );
    s.knotFrame.scale( s.scale ); 

    // Move Crystal Frame around knot orbit
    tp = s.crystalFrame.pos(); tp[3]= s.ecc;
    p1 = tp.spin(bst);
    p2 = p1.spin(bst);
    p1 = Round::loc(p1); p2 = Round::loc(p2); //renormalize 
    s.crystalFrame.pos() = p1;  s.crystalFrame.relOrient( p2, .5 );

  
    for (int i=0;i<NUMPARTICLES;++i){

        s.phasebin[i] = spectralInfo -> phasebin[i];
        s.magbin[i] = spectralInfo -> magbin[i];
        if ( fabs(s.magbin[i]) * s.amp  > .5) s.particles[i] = s.particles[i].spin ( bst );

    }

    // (UNUSED) Tangent at frame in direction along orbit
    //s.tangentA = Round::renormalize( Tangent::at( s.frame.pos() ^ tk.HF.fiberA().dual() , s.frame.pos() ) );
    //s.tangentB = Round::renormalize( Tangent::at( s.frame.pos() ^ tk.HF.fiberB().dual() , s.frame.pos() ) );

    //Knot step();
    s.knotFrame.step();

    //Diatom motif
    s.motif.step();

    //Crystal Frame
    s.cScale *= .9;
    s.crystalFrame.scale() = .001 + s.cScale;
    s.crystalFrame.step();

    //auto crystalHit = [this](){
    bCrystalHit = false;
    for (int i = 0; i<crystal.mPoint.size(); i++){//=crystal.mStride){
      auto& p1 = crystal.mPoint[i];
      auto dot1 = p1 <= Round::dls( crystal.mFrame.pos(), crystal.mFrame.scale() + .1 );//crystal.mFrame.bound();// Round::distance(p1,p2) ;
      auto dot2 = p1 <= Round::dls( crystal.mFrame.pos(), crystal.mFrame.scale() + .2 );
      if (dot2[0] > 0 && dot1[0] < 0 ) { //is inside outer bound but outside inner bound
         bCrystalHit = true;
         break;
      }
    }
    

    //Virtual Camera
    mCamera.step();

    // use virtual camera riding along know? if so do not draw tangent arrow
    if (s.bUseCam) {
      s.bDrawTangent = false;
      mApp->scene.camera.set( mCamera.pos(), mCamera.quat() ); 
    }



    
  }
  
  /// Update Audio Parameters (e.g. based on geometry)
  inline void User::updateAudio()  {}
  /// Receive OSC Messages
  inline void User::onMessage(al::osc::Message& m)  {}



/*-----------------------------------------------------------------------------
 *  GUI PARAMETERS
 *-----------------------------------------------------------------------------*/
namespace hs{
/// User Defined GUI Specifications
template<> template<>
void Param<float>::specify(User::Data& k){
  //---------label, dataPtr, min,max
  mData = {   
    //Knot
    {"frame_orbit_speed", &(k.frame_orbit_speed),.0001,100},
    {"frame_orbit_max_speed", &(k.frame_orbit_max_speed),.0001,100},
    {"particle_orbit_speed", &(k.particle_orbit_speed),.0001,100},
    {"p", &(k.p),0,100},
    {"q", &(k.q),0,100},
    {"scale", &(k.scale),1,10},
    {"num",&(k.num),0,1000},
    {"ecc",&(k.ecc),-10,10},
    {"numTrace",&(k.mNumTrace),1,100},
    {"tube_opacity",&k.tube_opacity,0,1},
    {"yoffset",&k.yoffset,0,10},
    {"amp",&(k.amp),-5000,5000},
    
    //Diatom ...

    //Crystal
    {"cp",&(k.cp),2,6},
    {"cq",&(k.cq),2,6},
    {"bravais",&(k.bravaisType),0,7},
    {"diatom_force",&(k.diatom_force),0,7},
    {"numX",&(k.numX),1,20},
    {"numY",&(k.numY),1,20},
    {"numZ",&(k.numZ),1,20},
    {"cMode",&(k.crystalMotifMode),0,10},
    {"cScale",&(k.cScale),-10,10},
    {"xratio",&(k.xratio),0,100},
    {"yratio",&(k.yratio),0,100},
    {"zratio",&(k.zratio),0,100},
    {"linewidth",&k.linewidth,1,30},
    {"pointsize",&k.pointsize,1,30}
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
    {"pqbar",&(k.pqbar)},

    //stuff
    {"bReset",&(k.bReset)},
    {"bDrawDiatom",&(k.bDrawDiatom)},
    {"bDrawKnot",&(k.bDrawKnot)},
    {"bDrawTube",&(k.bDrawTube)},
    {"bDrawCircles",&(k.bDrawCircles)},
    {"bDrawFibers",&k.bDrawFibers},
    {"bDrawTangent",&k.bDrawTangent},    
    {"bDrawParticles",&(k.bDrawParticles)},
    {"bDrawCrystal",&(k.bDrawCrystal)}
  };
}
}//hs::
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
