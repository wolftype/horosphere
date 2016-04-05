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


// using namespace al;
using namespace gam;
using namespace std;
using gam::Sine;

namespace hs {



struct AudioProcess : public gam::Process<gam::AudioIOData> {

  /// Master Mix of Process
  static float mMasterMix;
	float mMix = 0;

  string mName = "AudioProcess";

	AudioProcess() {}

  float mix() { return mMix * mMasterMix; }

	//Spatial Mix
	#ifdef __allosphere__
	  AlloMix src;
  #else
    DesktopMix src;
	#endif

};


//////////----------PROCESSES ---------------/////////////

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

		mMix = .1;

		#ifdef __allosphere__
		channel = 47;
    #else
		channel = 0;
		#endif

    c1 = 440;
    c2 = 440;
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
			io.out(0) += s * src[0] * mMix;
			io.out(1) += s * src[1] * mMix;
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

struct WindSound : public AudioProcess {

  NoisePink<> noise;
  Reson<> reson;

  float width, freq;

	WindSound(){
    mMix = .1;
    width = 100;
    freq = 10;
    mName = "WindSound";
	}


  virtual void onProcess(gam::AudioIOData& io) override {
    reson.width(width);
    reson.freq(freq);
    src();
    while (io()) {
      float s = noise();
      s = reson(s);
		  for (int i = 0; i < src.numChannels(); ++i ){
		    io.out(i) += s * src[i] * mMix;
		  }
    }
  }
};

/// Default Interface Settings
template<>
void AudioParam::specify(WindSound& ws){
  mData = {
    {"/width", &ws.width, 0,800 },
    {"/freq", &ws.freq, 200,800 }
  };
};

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
