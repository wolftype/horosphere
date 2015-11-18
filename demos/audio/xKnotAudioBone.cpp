/*
 * =====================================================================================
 *
 *       Filename:  xKnotAudioBone.cpp
 *
 *    Description:  xKnotAudio
 *
 *        Version:  1.0
 *        Created:  05/22/2015 19:36:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "horo_AudioProcess.h"   
#include "horo_GxSync.h"
#include "horo_AudioApp.h"         

#include "data/xKnot.h"
//#include "data/vsr_knotData.h"
//#include "data/vsr_audioData.h"

using namespace glv;
using namespace gam;
using namespace std;   


struct AudioData{

  float freq, width, nrg, ftheta, fphi, fwrither, ratio, nftheta, nfphi;

  void update( State * state ){
     State& s = *state;
     freq = 330. + ( 110. * ( 10 * s.diameter ) ); 
     width = 10 + ( 8000. * s.diameter );  
     nrg = s.energy / 100.;     /// Hovers around 1
     ftheta =  vsr::Vec(Op::pj( s.vec, Biv::xz)).norm();  
     fphi =  vsr::Vec(Op::pj( s.vec, Biv::xy)).norm();  
     fwrithe = s.writhe/6.0;    
     ratio = .01 * s.P / s.Q;
     nftheta = 40 + std::fabs(s.theta) * 40; 
     nfphi = 40 + std::fabs(s.phi) * 40; 
  }
};
              
struct MyApp : public AudioBone<State> { 
  
  vector< AudioProcess * > ap;

  MyApp() : AudioBone<State>() {
 
    cout << "CHANNELS: " << mAudioIO.channelsOut()  << endl;

    ap.push_back( &mScheduler.add<WindSound>() ); 
    ap.back() -> mix = .065; 
    ap.push_back( &mScheduler.add<Harmonics>() );  
    ap.back() -> mix = .005; 
    ap.push_back( &mScheduler.add<FMSynth>() );    
    ap.back() -> mix = .001; 

    for (int i = 0; i < ap.size(); ++i){   
      ap[i] -> initGui(glv.gui);  
    } 
      
    glv.gui.arrange(); 

  }

  //called onframe
  virtual void onAnimate(){
    //get state info and then update aproc


    for (int i = 0; i < ap.size(); ++i ){
      ap[i] -> update();
    }
  }   

  virtual void onSound(gam::AudioIOData& io) {
   Scheduler& s = io.user<Scheduler>();
   s.update(io);                
  }

  
// virtual void onMessage(al::osc::Message& m) {            
//    string ts = m.addressPattern();  
//	  unsigned pos = ts.find_last_of("/");
//    string ss = ts.substr(0,pos);
//
//	  float f;
//	  m >> f;
//
//	  for (int i = 0; i < ap.size(); ++i ){ 
//		  if (ss == ap[i] -> name ) {
//		   ap[i] -> mVarMap[ts] -> val = f;
//		  }
//   }       
//  }  


};
                                          

int main(int argc, char * argv[]) {

  MyApp app;
  app.start();
  return 0;
}
