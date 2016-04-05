///@filename Simulator controls state, outputs audio, has a gui, and listens to OSC messages


#ifndef HS_SIMULATOR_INCLUDED
#define HS_SIMULATOR_INCLUDED

#include "vsr/vsr_app.h"
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/protocol/al_OSC.hpp"

//ADD-ONS
#include "horo_OSCApp.h"
#include "hsAudio.hpp"
#include "hsAudioProcess.hpp"
#include "hsGui.hpp"

namespace hs {

  template<class T>
  struct Simulator : App, public OSCReceiver {

    //Mouse Position
    vsr::cga::Point mouse;

    cuttlebone::Maker<typename T::State> maker;
    T mParam;

    GuiMap glui;
    Audio audio;

    Simulator(const char * ip = "127.0.0.1") : maker(ip){} //"192.168.10.255"

    virtual void start() override {

      //Call T::setup();
      mParam.setup();

      //Start OSC, Cuttlebone, Audio and Graphics Threads
      OSCReceiver::init(8082);
      OSCReceiver::start();
      maker.start();
      audio.start();
      App::start();
    }

    virtual void onFrame() override {
      App::onFrame();
      maker.set( mParam.mState );
      OSCReceiver::listen();
    }

    virtual void onDraw() override {
      mouse = calcMouse3D();
      mParam.onDraw();
    }

    virtual void onMessage(al::osc::Message& m){
        auto ap = m.addressPattern();
        cout << ap << endl;
        if (ap == "/euler"){
            float x,y,z;
            m >> x; m >> y; m >> z;
            cout << x << " " << y << " " << z << endl;
        }
        if (ap == "/pitch"){
            float x;
            m >> x;
            cout << x << endl;
        }
        if (ap == "/yaw"){
            float x;
            m >> x;
            cout << x << endl;
        }
        if (ap == "/roll"){
            float x;
            m >> x;
            cout << x << endl;
        }
        mParam.onMessage(m);
    }

  };

} //hs::


#endif /* end of include guard: HS_SIMULATOR_INCLUDED */
