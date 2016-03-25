/*
 * =====================================================================================
 *
 *       Filename:  xAudio_GLV.cpp
 *
 *    Description:  audio with gui interface
 *
 *        Version:  1.0
 *        Created:  01/18/2016 13:20:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "Gamma/Gamma.h"
#include "Gamma/AudioIO.h"
#include "Gamma/Domain.h"
#include "Gamma/Scheduler.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Noise.h"
#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"

#include "vsr_app.h"
#include "horo_glv_gui.h"
//#include "horo_AudioProcess.h"
//#include "allocore/al_Allocore.hpp"

using namespace gam;




//tmp audioprocess class

struct AudioProcess : public gam::Process<AudioIOData> { 

  float mix; 

};

template<>
UI<AudioProcess> :: setup(){
	gmap.add("/mix");
	gmap.set("/mix",mix); 
}


struct WindSound2 : public AudioProcess {

  NoisePink<> noise;
  Reson<> reson;  
	
	WindSound2(string n = "/wind") : AudioProcess(){}
	
//	void setup(){  
//
//		add("/width",0,800);
//		add("/freq",200,1000);
//		set("/width", 100);  
//		 
//		AudioProcess::setup();   
//	}   
//	
//	void update(){  
//		reson.width ( get("/width") );
//		reson.freq (  get("/freq") ); 
//		
//		AudioProcess::update(); 
//	}
    
  void onProcess(gam::AudioIOData& io){
      while (io()) {
        float s = noise();//rnd::uniformS() * 0.05;
       // s = reson(s);
		    for (int i = 0; i < 2; ++i ){  
		      io.out(i) += s;// * src[i] * mix; 
		    } 
      }
   }
    
};



class AudioApp : public AudioCallback {
public:

	AudioApp(){
		mAudioIO.append(*this);
		initAudio(44100);
	}

	void initAudio(
		double framesPerSec, unsigned framesPerBuffer=128,
		unsigned outChans=2, unsigned inChans=0
	){

    //tether to scheduler
    mAudioIO.callback = Scheduler::audioCB<AudioIOData>;
	  mAudioIO.user(&mScheduler);

		mAudioIO.framesPerSecond(framesPerSec);
		mAudioIO.framesPerBuffer(framesPerBuffer);
		mAudioIO.channelsOut(outChans);
		mAudioIO.channelsIn(inChans);
		sampleRate(framesPerSec);
	}

	AudioIO& audioIO(){ return mAudioIO; }


	void start(bool block=true){
		mAudioIO.start();

		if(block){		
			printf("Press 'enter' to quit...\n"); getchar();
		}
	}
 
  Scheduler& scheduler() { return mScheduler; }

private:
	AudioIO mAudioIO;
  Scheduler mScheduler;


};


struct AlloApp : public App, public AudioApp  {

  void start(){
    
    //start audio but do not block
    AudioApp::start(false);
    
    //start graphics loop
    App::start();
        
  }

};

struct MyApp : public AlloApp {
  
  Sine<> sine = Sine<>(440);

  float freq;

  void onAudio(AudioIOData& io){
    updateAudio();
  }

  void setup(){
    bindGLV();
    
    gui(freq,"freq",100,800);
    scheduler().add<WindSound2>();
  }

  void updateAudio(){
    sine.freq( freq );
  }

  void onDraw(){
   // updateAudio();
  }
  
};


int main(){

  MyApp app;
  app.start();


}
