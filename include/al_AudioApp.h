
#ifndef AUDIOAPP_H_INCLUDED
#define AUDIOAPP_H_INCLUDED


#include "Gamma/Gamma.h"
#include "Gamma/AudioIO.h"
#include "Gamma/Scheduler.h"

#include "GLV/glv.h"  
#include "GLV/glv_binding.h"
#include "GLV/glv_util.h" 
#include "GLV/glv_layout.h"
#include "al_glv_gui.h"

#include "allocore/al_Allocore.hpp" 

#include "al_SharedData.h" 
#include "gam_GxSync.h" 


using namespace al;
using namespace gam;
using namespace std; 
      
using glv::GLV;

struct AudioApp : public al::Window {//, public osc::PacketHandler  {    

  GLVGui glv;
  bool bMute;
  float mMasterVolume;
	
	AudioApp(std::string name = "audioapp", bool slave=false)// :   
 // mOSCRecv(PORT_TO_DEVICE_SERVER),
//	mOSCSend(PORT_FROM_DEVICE_SERVER, MAIN_RENDERING_MACHINE)    
	{   
		
		#ifdef __allosphere__
		initAudio("AF12 x5", 44100, 256, 0, 60);   

		#endif
		#ifndef __allosphere__
		initAudio(44100, 256);
		#endif 
    
    initGLV();      
    initWindow();
 
		
	//	oscRecv().bufferSize(32000);
	//	oscRecv().handler(*this);
    		
	}                  
	
	virtual ~AudioApp();
		
	
  void initAudio( double audioRate=44100, int audioBlockSize=256 );
	
	void initAudio(
		std::string devicename, 
    double audioRate, int audioBlockSize,
		int audioInputs, int audioOutputs
	);

  void initWindow(){
    create( Window::Dim(800, 800), "AudioApp", 60, Window::DEFAULT_BUF );
  }
  
  void initGLV(){
    glv.parentWindow(*this);
    glv.gui.colors().back.set(.3,.3,.3);
     glv.gui.colors().border.set(1,0,0);
    glv.gui.enable( glv::DrawBorder );
   // glv.gui(bMute,"mute");
   // glv.gui(mMasterVolume,"master_volume");
  //  glv.gui.arrangement("x,x");
    //glv.gui.arrange();
  }

  virtual bool onFrame(){
    
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //while(oscRecv().recv()) {}  
    update();
    onDraw(); 
    
    return true;
  }
	
	void start(){   
	//	mOSCRecv.timeout(.01);     
	//	mOSCRecv.start();
		mAudioIO.start();
    MainLoop::start();
	} 
  
  virtual void update(){} 
  virtual void onDraw(){}
	
	static void AppAudioCB(gam::AudioIOData&); 
	
	// virtual void onDraw(Graphics& gl) {}
	virtual void onAnimate(double dt) {}
	virtual void onSound(gam::AudioIOData& io) {}   	
  virtual void onMessage(osc::Message& m){}  

  virtual bool onKeyDown(const al::Keyboard& k){ 
  
     if (k.key() == 'v') glv.gui.toggle( glv::Property::Visible );
     return true; 
  }  	
	
	/* osc::Recv&			oscRecv(){ return mOSCRecv; } */
	/* osc::Send&			oscSend(){ return mOSCSend; } */     
	  
	/* osc::Recv mOSCRecv; */
	/* osc::Send mOSCSend; */    
	//                           
	gam::AudioIO mAudioIO; 
  gam::Scheduler mScheduler; 
		
};                  

inline AudioApp::~AudioApp() {
}


inline void AudioApp::initAudio(
	double audioRate, int audioBlockSize
) {
	
    mAudioIO.callback = mScheduler.audioCB;
	  mAudioIO.user(&mScheduler);
    
	  mAudioIO.framesPerSecond(audioRate);
	  mAudioIO.framesPerBuffer(audioBlockSize);
    gam::Sync::master().spu( mAudioIO.fps() );
}

inline void AudioApp::initAudio( 
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

inline void AudioApp::AppAudioCB(gam::AudioIOData& io){
	AudioApp& app = io.user<AudioApp>();
	io.frame(0);
	app.onSound(io);
}        


#endif
