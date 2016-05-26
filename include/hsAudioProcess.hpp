/*
 * =====================================================================================
 *
 *       Filename:  hsAudioProcess.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/26/2014 16:08:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef VTX_AUDIO_PROCESSES_INCLUDED
#define VTX_AUDIO_PROCESSES_INCLUDED

#include "hsAudioSpatializer.hpp"
#include "hsData.hpp"

#include "Gamma/gen.h"
#include "Gamma/Scheduler.h"
#include "Gamma/Noise.h"
#include "Gamma/Effects.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"
#include "Gamma/DFT.h"


// using namespace al;
using namespace gam;
using namespace std;
using gam::Sine;

namespace hs {



struct AudioProcess : public gam::Process<gam::AudioIOData> {

	AudioProcess() {}
//	AudioProcess(const AudioProcess& a) {}

  float mix() { return mMix * mMasterMix; }
	
  virtual void onProcess(gam::AudioIOData& io) override {}

	//Spatial Mix
	#ifdef __allosphere__
	  AlloMix src;
  #else
    DesktopMix src;
	#endif

  /// Master Mix of Process
  static float mMasterMix;
	float mMix = 0;
  string mName = "AudioProcess";

};


//------------------------------------------------------------------------------

struct FMSynth : public AudioProcess {

	Sine<> car1;
	Sine<> car2;
	Sine<> mod1;
	Sine<> mod2;

	LFO<> lfo;

	float c1, c2;
	float m1, m2;
  float lfreq;

	AD<> ad;

	int channel;

  FMSynth()  {

		#ifdef __allosphere__
		channel = 47;
    #else
		channel = 0;
		#endif

    c1 = 150;
    c2 = 60;
    m1 = 20;
    m2 = 21;
    lfreq = 10;

    mName = "FMSynth";
	}

	virtual void onProcess(gam::AudioIOData& io) override {
    src();
		lfo.freq( lfreq );
    while (io()) {
			float f = lfo.para();
			car1.freq( c1 + m1 * f );
			car2.freq( c2 + m2 * f );

			float s = ( car1() + car2() );
			io.out(channel) += s * src[0] * mMix;
			//io.out(1) += s * src[1] * mMix;
		}
	}
};

/// Default Interface Settings
template<>
void AudioParam::specify(FMSynth& fm){
  mData = {
    {"/c1",&fm.c1, 40,800},
    {"/c2",&fm.c2, 60,800},
    {"/m1",&fm.m1, 1,400},
    {"/m2",&fm.m2, 1,400},
    {"/lfreq",&fm.lfreq,10,60}
  };

};


//------------------------------------------------------------------------------

struct WindSound : public AudioProcess {

  NoisePink<> noise;
  Reson<> reson;

  float width, freq;

	WindSound(){
    mMix = .06;
    width = 100;
    freq = 10;
    mName = "WindSound";
	}


  virtual void onProcess(gam::AudioIOData& io) override {
    reson.width(width);
    reson.freq(freq);
  //  src();
    while (io()) {
      float s = noise();
      s = reson(s);
		  for (int i = 0; i < src.numChannels(); ++i ){
		    io.out(i) += s * mMix;
		  }
    }
  }
};

/// Default Interface Settings
template<>
void AudioParam::specify(WindSound& ws){
  mData = {
    {"/width", &ws.width, 0,600 },
    {"/freq", &ws.freq, 100,800 }
  };
};

//------------------------------------------------------------------------------


struct Harmonics : public AudioProcess {

  // Generators
	ChebyN<16> shaper;
	Sine<> sine;
	LFO<> lfo;
	Comb<> comb = Comb<>(.01,1,0);

  // Parameters
  float sfreq, lfreq, lmod, cdelay, cfbck, cffd;

	Harmonics() {
    sfreq = 100;
    lfreq = 10;
    lmod = .5;
    cdelay = .01;
    cfbck = 1;
    cffd = 0;

		for (int i = 0; i < shaper.size(); ++i){
			shaper.coef(i) = 1./(i+1);
		}

    mName = "Harmonics";
	}

	void update(){
		sine.freq ( sfreq );
		lfo.freq ( lfreq );
		lfo.mod ( lmod );
		comb.set( cdelay, cfbck, cffd );
    src();
  }

	virtual void onProcess(gam::AudioIOData& io) override {

    update();

		while (io()){
			for (int i = 0; i < shaper.size(); ++i){
				shaper.coef(i) = 1./(i+1) * fabs( lfo.pulse() );
			}
			float s = sine();
			s = shaper(s) ;
		  s = comb (s);
			for (int i = 0; i < src.numChannels(); ++i ){
				io.out(i) += s * src[i] * mMix;
			}
		}
	}

};

template<>
void AudioParam::specify( Harmonics& hs){
  mData = {
    {"lmod",&hs.lmod,0,1},
    {"lfreq",&hs.lfreq,0,100},
    {"sfreq",&hs.sfreq,40,800},
    {"cfbck",&hs.cfbck,-1,1},
    {"cffd",&hs.cffd,-1,1},
    {"cdelay",&hs.cdelay,0,1}
  };
}


/// Modded From L. Putnam's Gamma Library how-to pdf
struct Echo : public AudioProcess{
 
  Echo() {
    init();
  }

  void init(){
    delayMax = .2;
    delay = .323;
    ffd = 0;
    fbk = .8; 
    mName = "Echo";  

  }
    
  void onProcess(AudioIOData& io){ 
   
    
    while(io()){
      echo.set(delay,ffd,fbk);
      echo.maxDelay(delayMax);


   //     float2 s = float2(io.out(0), io.out(1));
   //     s = echo(s)*0.5;
   //     io.out(0) += s[0];
   //     io.out(1) += s[1];

        for (int i = 0; i < src.numChannels(); ++i){
            float s = io.out(i);
            s = echo(s)*.5;
            io.out(i) = dc[i](s);
        }
    } 
  }
  
  Comb<float> echo;
  BlockDC<float> dc[60];
  float delayMax;
  float delay;
  float ffd;
  float fbk;
   
};

template<>
void AudioParam::specify( Echo& ap){

  mData = {
    {"delayMax",&ap.delayMax,.001,10},
    {"delay",&ap.delay,0,10},
    {"ffd",&ap.ffd,0,10},
    {"fbk",&ap.fbk,0,1}
  };

}


/// Modded From L. Putnam's Gamma Library how-to pdf
struct Voice : public AudioProcess {

  Voice(){
    init();
  }

  void init(){
    freq=690;
    phase = 0;
    mod = .5;
    attack = .2;
    decay = .6;
    //env.loop(true);
    loop = false;
    mName = "Voice";
    mMix = .5;
  }

  void onProcess(AudioIOData& io){ 
      osc.set(freq,phase,mod);
      env.attack(attack);
      env.decay(decay);
    env.loop((int)loop);
  //  src();
    while(io()){



      float s = val() * env() * .1;

		  for (int i = 0; i < src.numChannels(); ++i ){
		    io.out(i) += s *  mMix; // src
		  }      
    }
  }

  float val() { 
    switch((int)mode) {
      case 0: return osc.tri();
      case 1: return osc.sqr();
      case 2: return osc.cos();
      case 3: return osc.pulse();
      default: return osc.even3();
    }
  }

  enum {
    TRI =0, SQR=1, COS=2, PULSE=3, SIN=4
  };

  LFO<> osc; ///< oscillator  
  AD<> env;  ///< Envelope
  float mode = 0;
  float freq;
  float phase;
  float mod;
  float attack;
  float decay;
  float loop;

};

template<>
void AudioParam::specify( Voice& ap){
  mData = {
    //LFO
    {"freq",&ap.freq,40,2200},
    {"phase",&ap.phase,40,1200},
    {"mod",&ap.mod,0,10},
    {"mode",&ap.mode,0,5},
    //ENV
    {"attack",&ap.attack,0,1},
    {"decay",&ap.decay,0,1},
    {"loop",&ap.loop,0,1}
  };
}

/// Multiple Voices
struct Chord : AudioProcess {
  
  vector<LFO<>> note;
  AD<> env;  ///< Envelope
  
  /// set with base freq and steps above
  template<class ... T>
  void set(float freq, T ... num){
    note = { LFO<>(f(freq,(int)num)) ... };
  }

  virtual void onProcess(AudioIOData& io){

    env.attack(attack);
    env.decay(decay);

    while(io()){
      float te = env();
      for (auto& i : note){
        float s = i.tri() * te * .1;
        io.out(0) += s;
        io.out(1) += s;
      }
    }
  }
  
  float f( float base, int halfsteps) { return base * pow(root,halfsteps); }
  float root = 1.059463094359293; //pow(2, 1.f/12);
  float freq;
  float attack;
  float decay;
};

/// Extract info
struct SpectralInfo : AudioProcess {

	STFT stft;		// Short-time Fourier transform
  
  float * magbin; /// store magnitude after messing with it
  float * phasebin; /// store phase after messing with it

	SpectralInfo()
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

    mName = "SpectralInfo";
    magbin = new float[1025];//stft.numBins()];
    phasebin = new float[1025];//stft.numBins()];
  }

  void onProcess(AudioIOData& io){
    while(io()){
      float s = io.out(0);//, io.out(1));
      if (stft(s)){
        for (int k=0;k<stft.numBins();++k){
          magbin[k] = stft.bin(k).mag();
          phasebin[k] = stft.bin(k).phase();
        }
      } 
    }
  }
};

/// Take Noise and pick out select bands
struct SpectralNoise : AudioProcess {
  
	STFT stft;		// Short-time Fourier transform
	NoisePink<> noise;

	SpectralNoise()
	: stft(
		1024,		// Window size
		1024/4,		// Hop size; number of samples between transforms
		0,			// Pad size; number of zero-valued samples appended to window
		HANN,		// Window type: BARTLETT, BLACKMAN, BLACKMAN_HARRIS,
					//		HAMMING, HANN, WELCH, NYQUIST, or RECTANGLE
		COMPLEX		// Format of frequency samples:
					//		COMPLEX, MAG_PHASE, or MAG_FREQ
	)
	{


    mName = "SpectralNoise";

  }

  void onProcess(AudioIOData& io){ 

   // cout << stft.numBins() << endl;
    while(io()){

      float s = noise();
  // Input next sample for analysis
  		// When this returns true, then we have a new spectral frame
      if (stft(s)){
  //      
  //				// Loop through all the bins
  				for(unsigned k=0; k<stft.numBins(); ++k){
  //
  				  	// Band Pass
              if (k > max || k < min ) stft.bin(k) = 0;
              // Noise Floor threshold
              if (stft.bin(k).mag() < thresh) stft.bin(k) = 0;
  //
  				}
      }

      // Get next resynthesized sample
  		s = stft();
  		
		  for (int i = 0; i < src.numChannels(); ++i ){
		    io.out(i) += s * mMix;
		  }

    }
  }


  float max;
  float min;
  float thresh;
  
};

template<>
void AudioParam::specify( SpectralNoise& ap){
  mData = {
    //LFO
    {"max", &ap.max,0,1024},
    {"min", &ap.min,0,1024},
    {"thresh",&ap.thresh,0,1},
  };
}

/// Get Maximum Amplitude, if above threshold set trigger
struct MagnitudeReader : AudioProcess {

  MagnitudeReader(){
    mName = "mag";
  }
  
  virtual void onProcess(AudioIOData& io) {
    max = 0;
    bTrigger = false;
    while(io()){
      float tmp = io.out(0);
      if (tmp > max) max = tmp;
    }
    if (max > thresh) bTrigger = true;
    
  }

  bool bTrigger;
  float max;
  float thresh;   /// threshold for spitting out mag
};

template<>
void AudioParam::specify( MagnitudeReader& ap){
  mData = {
    //LFO
    {"thresh",&ap.thresh,0,1}
  };
}

//------------------------------------------------------------------------------


/*
struct WindSound : public AudioProcess {

    NoisePink<> noise;
    Reson<> reson;

	WindSound(string n = "/wind") : AudioProcess(n){
		cout << src.pos << endl;
	}

	void setup(){

		add("/width",0,800);
		add("/freq",200,1000);
		set("/width", 100);

		AudioProcess::setup();
	   // reson.width (100);
	   //cout << "NUM: " << src.speakers.num() << endl;
	}

	void update(){
   // cout << "wind" << endl;
		reson.width ( get("/width") );
		reson.freq (  get("/freq") );

		AudioProcess::update();
	}

    void onProcess(gam::AudioIOData& io){
        while (io()) {
          float s = noise();//rnd::uniformS() * 0.05;
          s = reson(s);
			    for (int i = 0; i < src.speakers.num() -1; ++i ){
			      io.out(i) += s * src[i] * mix;
			    }
        }
    }

};

struct TwistSound : public AudioProcess {

    Saw<> car;
    Sine<> mod;

	TwistSound(string n = "/twist") : AudioProcess(n) { mod.freq(10); }

	void setup(){}
	void update(){}

    void onProcess(gam::AudioIOData& io){
        while (io()) {
            car.freq( 340 + ( mod() * 100 )  );
            //cout << car.freq() << endl;
            float s = car();//rnd::uniformS() * 0.05;
            //s = reson(s);
            io.out(0) += s;
        }
    }

};

struct EventSound : public AudioProcess {
    AD<> env;
    Sine<> car;
	gen::Trigger trigger;

	//bool trigger;

	EventSound(string n  = "/event") : AudioProcess(n ), trigger(10000) {
		car.freq(601);
	}//env.loop(true); }

	void setup(){

		AudioProcess::setup();

		add("/freq", 100, 800);
		add("/tfreq", 10, 10000);
		add("/attack",0,10000);
		add("/decay",0,10000);
		//add("bTrigger");
	}

	void update(){

		car.freq( get("/freq") );	//mVarMap["cfreq"] -> val );
		env.attack( get("/attack"));
		env.decay( get("/decay"));
		trigger.num =  get("/tfreq") * 100;

	    AudioProcess::update();
		//if ( ( mVarMap["bTrigger"] -> val ) > .5 ) env.reset();
	}

    void onProcess(gam::AudioIOData& io){
         while (io()) {

			if (trigger()) {
				env.reset();
			}
			float s = car() * env();

			for (int i = 0; i < src.speakers.num(); ++i){
            	io.out(i) += s * src[i] * mix;
			}
         }
    }
};


struct Harmonics : public AudioProcess {

	ChebyN<16> shaper;
	Sine<> sine;
	LFO<> lfo;
	Comb<> comb;

	Harmonics(string n = "/harmonics") :  AudioProcess(n),
	comb(1./100.,1,0)
	{

		sine.freq(100);
		lfo.freq(10);
		lfo.mod( .5);
		comb.delay(.01);

		for (int i = 0; i < shaper.size(); ++i){
			shaper.coef(i) = 1./(i+1);
		}
	}

	void update(){
		sine.freq ( get("/sfreq") );//(*mVarMap["sfreq"] ).val );
		lfo.freq ( get("/lfreq"));//(*mVarMap["lfreq"]).val);
		lfo.mod ( get("/lmod") );//(*mVarMap["lmod"]).val );

		comb.set( get("/cdelay"), get("/cfbck"), get("/cffd") );
		AudioProcess::update();

	}

	void setup(){

		add("/lmod",0, 1);
		add("/lfreq", 0, 100);
		add("/sfreq", 40, 800);
	  add("/cfbck", -1, 1);
	 	add("/cffd", -1, 1);
	 	add("/cdelay", 0, 1);

		set("/sfreq",80);
		set("/cdelay",.012);

		AudioProcess::setup();

	}

	void onProcess(gam::AudioIOData& io){

		while (io()){
			for (int i = 0; i < shaper.size(); ++i){
				shaper.coef(i) = 1./(i+1) * fabs( lfo.pulse() );
			}
			float s = sine();
			s = shaper(s) ;
			s = comb (s);
			for (int i = 0; i < src.speakers.num(); ++i ){
				io.out(i) += s * src[i] * mix;
			}
		}
	}

};

struct Mix : public AudioProcess {

	double amp; // must be doubles!

	int mNumChannels; // <-- define here?
	Mix( int nc, string n = "/mix" ) : AudioProcess(n), mNumChannels(nc){
		amp = 0.0;
	}

	void update(){ amp = get( "/amp" ); }
	void setup() { add("/amp", 0.0, 1.0); }

	void onProcess(gam::AudioIOData& io){

		while (io()){
			for (int i = 0; i < mNumChannels; ++i){
				float s = io.out(i);
				io.out(0) = 0;//s * amp;
			}
		}
	}
};
*/

}//hs::
#endif
