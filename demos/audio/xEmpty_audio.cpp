/*
 * =====================================================================================
 *
 *       Filename:  xEmpty_audio.cpp
 *
 *    Description:  gets state from a simulator and renders audio
 *
 *        Version:  1.0
 *        Created:  05/22/2015 19:16:50
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

using namespace hs;

struct MyApp : App {

  GuiMap gm;
  Audio aud;

  void setup(){
    bindGLV();

    auto& fm = aud.mScheduler.add<FMSynth>();
    auto& ws = aud.mScheduler.add<WindSound>();
    auto& hs = aud.mScheduler.add<Harmonics>();

    gm.bind<AudioParam>(fm,gui);
    gm.bind<AudioParam>(ws,gui);
    gm.bind<AudioParam>(hs,gui);
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
