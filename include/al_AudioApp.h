
#ifndef AUDIOAPP_H_INCLUDED
#define AUDIOAPP_H_INCLUDED


#include "allocore/al_Allocore.hpp"
#include "allocore/io/al_Window.hpp"
#include "allocore/protocol/al_OSC.hpp"
#include "alloutil/al_FPS.hpp"              

#define PORT_TO_DEVICE_SERVER (12000)
#define PORT_FROM_DEVICE_SERVER (PORT_TO_DEVICE_SERVER+1)
#define DEVICE_SERVER_IP_ADDRESS "BOSSANOVA"    


struct AudioApp : 
public Window, public osc::PacketHandler, public FPS, public Drawable {    
	
	AudioApp(std::string name = "twistor_audio", bool slave=false) : 
	mOSCRecv(PORT_FROM_DEVICE_SERVER),
	mOSCSend(PORT_TO_DEVICE_SERVER, DEVICE_SERVER_IP_ADDRESS)
	{
	   	Window::append(mStdControls);
		initWindow();
		initAudio();
		oscRecv().bufferSize(32000);
		oscRecv().handler(*this); 
	}                  
	
	virtual ~AudioApp();
	
	void initWindow(
		const Window::Dim& dims = Window::Dim(800, 400),
		const std::string title = "AudioApp",
		double fps = 60,
		Window::DisplayMode mode = Window::DEFAULT_BUF);
			
	void initAudio(
		double audioRate=44100, int audioBlockSize=256
	);
	
	void initAudio(
		std::string devicename, 
        double audioRate, int audioBlockSize,
		int audioInputs, int audioOutputs
	);
	
	void start(){
		mAudioIO.start(); 
		create(); 
		MainLoop::start();
		//Main::get().start();
	}  
	
	static void AppAudioCB(gam::AudioIOData&); 
	
	virtual void onDraw(Graphics& gl) {}
	virtual void onAnimate(al_sec dt) {}
	virtual void onSound(gam::AudioIOData& io) {}
	virtual void onMessage(osc::Message& m){}  
	
	virtual bool onCreate(){}
	virtual bool onFrame();
	
	osc::Recv&			oscRecv(){ return mOSCRecv; }
	osc::Send&			oscSend(){ return mOSCSend; }     
	
	const Graphics&		graphics() const { return mGraphics; }
	Graphics&			graphics(){ return mGraphics; }
	              
   
	osc::Recv mOSCRecv;
	osc::Send mOSCSend;
 
	gam::AudioIO mAudioIO; 
    gam::Scheduler mScheduler; 

	Graphics mGraphics;   
	   
   	StandardWindowKeyControls mStdControls; 
		
};                  

inline AudioApp::~AudioApp() {
 //   if (!bSlave) sendDisconnect();
}

inline void AudioApp::initWindow(
	const Window::Dim& dims, 
	const std::string title, 
	double fps, Window::DisplayMode mode) 
	{
		Window::dimensions(dims);
		Window::title(title);
		Window::fps(fps);
		Window::displayMode(mode);
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

inline bool AudioApp::onFrame() {
	FPS::onFrame();

	while(oscRecv().recv()) {}
	
	onAnimate(dt);
	
	Viewport vp(width(), height());
	
	return true;
}   

#endif 