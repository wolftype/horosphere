/*
 * =====================================================================================
 *
 *       Filename:  horo_AudioProcess.h
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

#include "horo_AudioSpatializer.h"
 
#include "Gamma/gen.h"
#include "Gamma/Scheduler.h"
#include "Gamma/Noise.h"
#include "Gamma/Effects.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"

#include "GLV/glv.h"  
#include "GLV/glv_binding.h"
#include "GLV/glv_util.h"

#include "al_glv_gui.h"   


// using namespace al;     
using namespace gam;
using namespace std; 
using namespace glv;
using gam::Sine;

struct AudioGui : public View { 
	glv::Gui glv;
	AudioGui() : glv() {
		*this << glv;
		glv.colors().back.set(.3,.3,.3);
	}
};

struct AudioParam {
	
	double val;
	
	double min, max;
	
	AudioParam ( const AudioParam& p ) : val (p.val), min( p.min), max(p.max) {}
	
	AudioParam ( double _min = 0.0, double _max = 1.0 ) :
	val(0), min(_min), max(_max) {}

//	AudioParam(bool b) : val(0), min(0), max(1), trigger(b){}
	
};    

template<int N>
struct SpatialSource;
  
template< int N > 
ostream& operator << (ostream& os, SpatialSource<N>& s);   

template< int N >
struct SpatialSource {
  
  vector<float> f;
	Speakers< N > speakers;
	gfx::Vec3f pos;  
	
	int num() { return N; }   
	
	void operator() (){  	
		f = speakers.mix( pos );
	} 
	float operator[] (int idx) const{
		return f[idx];
	} 
	
	template<int S>  
	friend ostream& operator << ( ostream& os, SpatialSource<S> &s); 
};  
  
template<int N>
inline ostream& operator << ( ostream& os, SpatialSource<N> & s){
	for (int i = 0; i < s.speakers.num(); ++i ){
		os << "mix at: " << i << " is " << s[i] << " \n";
	} 
	return os;
}

struct AudioProcess : public gam::Process { 
	
    static float MasterMix;
    
	AudioProcess( string _name, double dt = 0 ) : Process(0), name (_name), mix(0) { 
		mix = 0;  	
		src();
	}
        

	float mix; 
    
  float mmix() { return mix * MasterMix; }
	
	//Spatial 
	#ifdef __allosphere__
	  SpatialSource< ALLO_SPEAKER_LAYOUT > src;  
	#endif
	#ifndef __allosphere__
    SpatialSource< 2 > src; 
	#endif

	typedef std::map<string, AudioParam*> VarMap;
	typedef VarMap::iterator mVarMapIt;
	VarMap mVarMap;   
	
	string name;	// Message namespace

	//add string to map for gui
	void add(string s, double min, double max){
		AudioParam * p = new AudioParam(min, max);
		mVarMap[name+s] = p;
	}
	//add string to map for gui
	void add(string s){
		AudioParam * p = new AudioParam();
		mVarMap[name+s] = p;
	}
	
	virtual void setup() {
		add("/mix");
		add("/x", -100,100);
		add("/y", -100,100);
		add("/z", -100,100); 
		
		set("/mix",mix); 
	};

	virtual void update() {
		mix = get("/mix"); 
		
		src.pos[0] = get("/x");
		src.pos[1] = get("/y");
		src.pos[2] = get("/z"); 
		
		src();  
		
		cout << src << endl; 
	};
	
	// double get(string s) const{
	// 	return mVarMap[name+s] -> val;
	// } 
      
	double get(string s){
		return mVarMap[name+s] -> val;
	}
	
	void set(string s, float f){
		mVarMap[name+s] -> val = f;
	}
	
	void initGui(Gui& g){	

		cout << "INIT GUI" << endl;

		setup();
	
		for ( mVarMapIt i = mVarMap.begin(); i != mVarMap.end(); i++){
			cout << (i->first) << " " << (i -> second -> val) << " " << (i -> second -> min) << " " << (i -> second -> max) << endl; 
			g( i -> second -> val, i -> first, i -> second -> min, i -> second -> max );
		}
	}
	 
	
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
	
	AD<> ad; 
	
	int channel;
	
	FMSynth( string n = "/fm") : AudioProcess(n){
		mix = 0;
		car1.freq(440);
		car2.freq(441);
		mod1.freq( 20 );
		mod2.freq( 21 );  
		lfo.freq(10);  
		#ifdef __allosphere__
		channel = 47;         
		#endif
		#ifndef __allosphere__
		 channel = 0; 
		#endif
	}
	    
	void setup(){ 
		 
		add("/c1", 40,800);
	   // add("/c1ph");  
		add("/c2", 40,800); 
	   // add("/c2ph");   
		add("/m1",1,400); 
		add("/m2",1,400);
	   // add("/mod1", 1, 800 );
	   // add("/mod2", 1, 800 ); 
		add("/lfreq",1,100);  
		 
		set("/c1",100);
		set("/c2",90);
		set("/lfreq",10);  
		set("/m1",10);
		set("/m2",10);  
		
		 AudioProcess::setup();
	}   
	void update(){ 
		
		c1 = get("/c1");  
		c2 = get("/c2"); 
		m1 = get("/m1");
		m2 = get("/m2");  
		
		//car1.phase( get("/c1ph"));
		//car2.phase( get("/c2ph"));  
	  //  mod1.freq( get("/mod1"));   
	  //  mod2.freq( get("/mod2")); 
		
		lfo.freq( get("/lfreq"));
		
		AudioProcess::update();
	}  
	
	void onProcess(gam::AudioIOData& io){
        while (io()) { 
			float f = lfo.para();
			car1.freq( c1 + m1 * f ); 
			car2.freq( c2 + m2 * f );  
  	
			float s = ( car1() + car2() );  
			io.out(channel) = s * mix;
		}
	}
};

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
    cout << "wind" << endl; 
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

#endif
