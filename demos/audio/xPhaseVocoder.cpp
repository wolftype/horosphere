/*
 * =====================================================================================
 *
 *       Filename:  xPhaseVocoder.cpp
 *
 *    Description:  attempt to make a phase vocoder with luke
 *
 *        Version:  1.0
 *        Created:  05/07/2016 17:15:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#include "vsr/vsr_app.h"
#include "hsAudio.hpp"
#include "hsAudioProcess.hpp"
#include "hsGui.hpp"

#include "Gamma/DFT.h"

using namespace hs;


struct PhaseVocoder : AudioProcess {
  
	STFT stft;		// Short-time Fourier transform
	NoisePink<> src;

  float prebin[512]; /// store magnitude before messing with it
  float postbin[512]; /// store magnitude after messing with it
  float phasebin[512]; /// store magnitude after messing with it

  vector<int> scale; /// bias towards these

	PhaseVocoder()
	: stft(
		2048,		// Window size
		2048/4,		// Hop size; number of samples between transforms
		0,			// Pad size; number of zero-valued samples appended to window
		HANN,		// Window type: BARTLETT, BLACKMAN, BLACKMAN_HARRIS,
					//		HAMMING, HANN, WELCH, NYQUIST, or RECTANGLE
		COMPLEX		// Format of frequency samples:
					//		COMPLEX, MAG_PHASE, or MAG_FREQ
	)
	{
    
    freq = 440;  

  }

  void onProcess(AudioIOData& io){ 
    
   // src.freq(freq);
    scale.clear();
    int n = base;
    for (int i = 0; i < 5; ++i){
     scale.push_back(n);
     n = n * 2;
   }
    
    while(io()){

      float s = src();
  		// Input next sample for analysis
  		// When this returns true, then we have a new spectral frame
      if (stft(s)){
  //      
  //				// Loop through all the bins
  				for(unsigned k=0; k<stft.numBins(); ++k){
  //
  //          prebin[k] = stft.bin(k).mag();
  //          phasebin[k] = stft.bin(k).phase();
  //
  				  	// Band Pass
              if (k > max || k < min ) stft.bin(k) = 0;
              // Noise Floor threshold
              if (stft.bin(k).mag() < thresh) stft.bin(k) = 0;
  //
  //          postbin[k] = stft.bin(k).mag();
  				}
      }

      // bias
      for(unsigned k=0; k<scale.size(); ++k){
        stft.bin(k) *= 2;
      }
  
      // Get next resynthesized sample
  		s = stft();
  		
  		io.out(0) = s;
  		io.out(1) = s;

    }
  }


  float freq;
  float max;
  float min;
  float thresh;
  float base;

  
};

template<>
void AudioParam::specify( PhaseVocoder& ap){
  mData = {
    //LFO
    {"freq",&ap.freq,40,1200},
    {"max", &ap.max,0,512},
    {"min", &ap.min,0,512},
    {"thresh",&ap.thresh,0,1},
    {"base", &ap.base, 0,16}
  };
}

struct MyApp : App {

  GuiMap gm;
  Audio aud;

  void setup(){
    bindGLV();

    auto& fm = aud.mScheduler.add<PhaseVocoder>();

    gm.bind<AudioParam>(fm,gui);
  }

  void onDraw(){

  }

};

/*-----------------------------------------------------------------------------
 *  MAIN LOOP
 *-----------------------------------------------------------------------------*/
int main(){

  MyApp app;
  app.aud.start();
  app.start();

  while(true){}

  return 0;
}
