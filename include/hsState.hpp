#pragma once

#include "vsr/space/vsr_cga3D.h"
#include "gfx/gfx_scene.h"

struct App;


namespace hs {

template<class T> struct Simulator;

/// BASE Virtual Base Class to be inherited by template parameter T
struct UserBase {
  
  using Process = gam::Process<Audio::IODATA>;

  App * mApp;

  /// Cast to Simulator typed on User's Data
  template<class T>
  Simulator<T>& simulator(T * u, void * s){ 
    auto& sim = *((Simulator<T>*)(s));
    return sim; 
  }

  /// Init State, Add AudioProcesses to Scheduler
  virtual void onSetup(void *) = 0;
  /// Draw State
  virtual void onDraw() = 0;
  /// Update Local Parameters on each Computer
  virtual void updateLocal() =0;
  /// Update Global Shared Parameters to Push
  virtual void updateGlobal() =0;
  /// Update Audio Parameters Based on Global Data
  virtual void updateAudio() =0;
  /// Listen for OSC messages from iPad
  virtual void onMessage(al::osc::Message& m) = 0;
};

/// DATA Encapsulation of Scene Data and User Data State
/// T must inherit from UserBase
template<class T>
struct UserData : T {


  /// State info for broadcasting
  struct State {

    /// Basic Scene Data to share
    struct SceneData{
      float time;
      float eyesep;
      gfx::Pose model;
      gfx::Pose camera;
      cga::Point mouse;
    } mSceneData;

    /// User's own Data to Distribute
    typename T::Data mData;

  } mState;

  /// Bind address of T::*mData to this->mState.mData;
  void bind(App * app = NULL){
    this->mData = &(this->mState.mData);
    if (app) this->mApp = app;
  }

};

} // hs::
