///@filename Simulator controls state, outputs audio, has a gui, and listens to OSC messages
/// it does some binding under the hood for automatic gui generation too

#ifndef HS_SIMULATOR_INCLUDED
#define HS_SIMULATOR_INCLUDED

#include "vsr/vsr_app.h"
#include "Cuttlebone/Cuttlebone.hpp"
#include "vsr/util/vsr_stat.h"


//ADD-ONS
#include "hsOSC.hpp"
#include "hsAudio.hpp"
#include "hsAudioProcess.hpp"
#include "hsGui.hpp"
#include "hsState.hpp"

//Control Flow
#include "ohio.hpp"


namespace hs {

  template<class T>
  struct Simulator : App, public al::OSCReceiver {

    /// Mouse Position
    vsr::cga::Point mouse;

    /// Wrap User's Struct and Add SceneData (camera pose, model pose) To it
    UserData<T> mUser;
    cuttlebone::Maker< typename UserData<T>::State > maker;//typename T::State> maker;

    //GuiMap glui;
    Audio audio;

    //Gamma filter to smoothen incoming OSC signals
    gam::MovingAvg<> maFilter;

    Simulator(const char * ip = "127.0.0.1") : maker(ip){} //"192.168.10.255"

    /// add process to scheduler at root and bind parameters to gui (so do not delete process!) 
    template<class AP>
    AP& addAudioProcess(){
      auto& tmp = audio.mScheduler.add<AP>();
      //glui.bind<AudioParam>(tmp,gui);
      Glui<AudioParam>::Bind(tmp,gui);
      return tmp;
    }

    /// add process scheduler at process node and bind parameters to gui (so do not delete process!) 
    template<class AP>
    AP& addAudioProcess(Audio::ProcessNode& proc ){
      auto& tmp = audio.mScheduler.add<AP>(proc);
      //ap = &tmp;
      //glui.bind<AudioParam>(tmp,gui); /// maybe these audio params in hsData.hpp should be shared ptrs
      Glui<AudioParam>::Bind(tmp,gui);
      return tmp;
    }

    /// Setup GLV binding to window and call User's setup code, passing in pointer to this
    virtual void setup() override{
      bindGLV();
      mUser.onSetup(this);
    
      //glui.bind<Param<float>>( mUser.mState.mData, gui);
      //glui.bind<Param<bool>>( mUser.mState.mData, gui);    
      Glui<Param<float>>::Bind( mUser.mState.mData, gui); 
      Glui<Param<bool>>::Bind( mUser.mState.mData, gui); 
      Glui<Param<float>>::Bind( mUser, gui); 
     // Glui<Param<bool>>::Bind( mUser, gui); 
    }

    virtual void start() override {

      ///Seed random number generator
      vsr::Rand::Seed();
      mUser.bind(this);
      
      //Start OSC, Cuttlebone, Audio and Graphics Threads
      OSCReceiver::init(8082);
      OSCReceiver::start();
      maker.start();
      audio.start();
      App::start();
    }

    virtual void onFrame() override {
      App::onFrame();
      maker.set( mUser.mState );
      OSCReceiver::listen();
    }

    virtual void onDraw() override {
      mouse = calcMouse3D();
      mUser.onDraw();
    }

    /// Here is where we can tie Audio Parameters to Global State
    virtual void onAnimate() override {
      auto &s = mUser.mState.mSceneData;
      s.time += .01;
      s.mouse = mouse;
      s.camera = (gfx::Pose)scene.camera;
      s.model = scene.model;

      mUser.updateLocal();
      mUser.updateGlobal();
      mUser.updateAudio();
    }


    virtual void onMessage(al::osc::Message& m){
        auto ap = m.addressPattern();
        //cout << ap << endl;
        if (ap == "/euler"){
            float x,y,z;
            m >> x; m >> y; m >> z;
        //    cout << x << " " << y << " " << z << endl;
        }
        if (ap == "/pitch"){
            float x;
            m >> x;
        //    cout << x << endl;
        }
        if (ap == "/yaw"){
            float x;
            m >> x;
        //    cout << x << endl;
        }
        if (ap == "/roll"){
            float x;
            m >> x;
        //    cout << x << endl;
        }
        mUser.onMessage(m);
    }

  };

} //hs::


#endif /* end of include guard: HS_SIMULATOR_INCLUDED */
