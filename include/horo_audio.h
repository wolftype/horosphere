/*
 * =====================================================================================
 *
 *       Filename:  horo_audio.h
 *
 *    Description:  audio only render
 *
 *        Version:  1.0
 *        Created:  05/22/2015 19:18:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */
#ifndef HORO_AUDIO_H_INCLUDED
#define HORO_AUDIO_H_INCLUDED

//#include "horo_AudioApp.h"
#include "vsr/vsr_app.h"
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/protocol/al_OSC.hpp"


//ADD-ONS
#include "horo_OSCApp.h"
#include "horo_GxSync.h" 

#include "Gamma/Gamma.h"
#include "Gamma/AudioIO.h"
#include "Gamma/Scheduler.h"


template<class TState>
struct AudioBone : App, public OSCReceiver {

  cuttlebone::Taker<TState> taker;
  TState * state;

  bool bMute;
  float mMasterVolume;
	gam::AudioIO mAudioIO; 
  gam::Scheduler mScheduler; 
		  

  AudioBone(){}; 

  void start(){
    
    state = new TState;
    memset(state, 0, sizeof(TState)); 
    taker.start();
    
  	#ifdef __allosphere__
		initAudio("AF12 x5", 44100, 256, 0, 60);   
		#endif
		
    #ifndef __allosphere__
		initAudio(44100, 256);
		#endif 

    //OSC
    OSCReceiver::init(8082);
    OSCReceiver::start();
    //Audio
  	mAudioIO.start();
    //Main
    App::start();
  }

  void onFrame(){
    App::onFrame();
    int popCount = taker.get(*state); 
    OSCReceiver::listen();
  }

  virtual void onMessage(al::osc::Message& m){
      auto ap = m.addressPattern();
      if (ap == "/euler"){
          double x,y,z;
          m >> x; m >> y; m >> z;
          cout << x << " " << y << " " << z << endl;
      }
  }

  void initAudio( double audioRate=44100, int audioBlockSize=256 );
	
	void initAudio(
		std::string devicename, 
    double audioRate, int audioBlockSize,
		int audioInputs, int audioOutputs
	);
  
 	static void AudioCB(gam::AudioIOData&); 
 	virtual void onSound(gam::AudioIOData& io) {}   	
};

	  	
inline void AudioBone::initAudio(
	double audioRate, int audioBlockSize
) {
	
    mAudioIO.callback = mScheduler.audioCB;
	  mAudioIO.user(&mScheduler);
    
	  mAudioIO.framesPerSecond(audioRate);
	  mAudioIO.framesPerBuffer(audioBlockSize);
    gam::Sync::master().spu( mAudioIO.fps() );
}

inline void AudioBone::initAudio( 
	std::string devicename,
	double audioRate, int audioBlockSize,
	int audioInputs, int audioOutputs
) {
	gam::AudioDevice indev(devicename, gam::AudioDevice::INPUT);
	gam::AudioDevice outdev(devicename, gam::AudioDevice::OUTPUT);
	indev.print();
	outdev.print();
	mAudioIO.deviceIn(indev);
	mAudioIO.deviceOut(outdev);
	mAudioIO.channelsOut(audioOutputs);
	mAudioIO.channelsIn(audioInputs);
	initAudio(audioRate, audioBlockSize);
}

inline void AudioBone::AudioCB(gam::AudioIOData& io){
	AudioApp& app = io.user<AudioApp>();
	io.frame(0);
	app.onSound(io);
}        


#endif




