/*
 * =====================================================================================
 *
 *       Filename:  horo_control.h
 *
 *    Description:  allosphere controller app, broadcasts state and listens locally for incoming OSC messages

          use: subclass this and fill state data in onAnimate loop
 *
 *        Version:  1.0
 *        Created:  05/22/2015 16:46:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */


#ifndef  horo_control_INC
#define  horo_control_INC

#include "vsr_app.h"
#include "Cuttlebone/Cuttlebone.hpp"
#include "allocore/protocol/al_OSC.hpp"

//ADD-ONS
#include "horo_OSCApp.h"

template<class TState>
struct ControlBone : App, public OSCReceiver {

  cuttlebone::Maker<TState> maker;
  TState * state;

  ControlBone(const char * ip = "127.0.0.1") : maker(ip){} //"192.168.10.255" 

  void start(){
    
    state = new TState;
    memset(state, 0, sizeof(TState)); 
    maker.start();
    
    OSCReceiver::init(8082);
    OSCReceiver::start();
    
    App::start();
  }

  void onFrame(){
    App::onFrame();
    maker.set(*state);
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
  
};


#endif   /* ----- #ifndef horo_control_INC  ----- */
