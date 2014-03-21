/*
 * =====================================================================================
 *
 *       Filename:  xKnotAudio.cpp
 *
 *    Description:  Audio For Knot, listens to data from main
 *
 *        Version:  1.0
 *        Created:  02/26/2014 15:50:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#include "horo_AudioProcess.h"   
#include "gam_GxSync.h"
#include "al_AudioApp.h"         

#include "al_SharedData.h"
#include "data/vsr_knotData.h"
#include "data/vsr_audioData.h"

using namespace glv;
using namespace gam;
using namespace std;   
              
struct MyApp : public AudioApp { 
  
  vector< AudioProcess * > ap;

  MyApp() : AudioApp() {
 
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

virtual ~MyApp() {}

// CALLED ONANIMATE
virtual void update(){
  for (int i = 0; i < ap.size(); ++i ){
    ap[i] -> update();
  }
}   

virtual void onDraw() {}

 virtual void onSound(gam::AudioIOData& io) {
  Scheduler& s = io.user<Scheduler>();
  s.update(io);                
 }

  
 virtual void onMessage(al::osc::Message& m) {            
    string ts = m.addressPattern();  
	  unsigned pos = ts.find_last_of("/");
    string ss = ts.substr(0,pos);

	  float f;
	  m >> f;

	  for (int i = 0; i < ap.size(); ++i ){ 
		  if (ss == ap[i] -> name ) {
		   ap[i] -> mVarMap[ts] -> val = f;
		  }
   }       
 }  


};
                                          

int main(int argc, char * argv[]) {

  MyApp app;
  app.start();
  return 0;
}
