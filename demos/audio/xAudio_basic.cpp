/*
 * =====================================================================================
 *
 *       Filename:  xAudio_basic.cpp
 *
 *    Description:  just running audio

        NOTES: AudioApp inherits from AudioCallback 

        similar to how GFXApp inherits from RenderNode

 *
 *        Version:  1.0
 *        Created:  12/21/2015 14:47:03
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

using namespace gam;

class AudioApp : public AudioCallback{
public:

	AudioApp(){
		mAudioIO.append(*this);
		initAudio(44100);
	}

	void initAudio(
		double framesPerSec, unsigned framesPerBuffer=128,
		unsigned outChans=2, unsigned inChans=0
	){
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

private:
	AudioIO mAudioIO;
};


struct MyApp : public AudioApp {
  
  Sine<> sine = Sine<>(440);

  void onAudio(AudioIOData& io){
    while(io()){
      io.out(0) = sine();
    }
  }
  
};


int main(){

  MyApp app;
  app.start();


}
