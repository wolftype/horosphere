/*
 * Copyright (C) 2010  Pablo Colapinto
 * All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * =============================================================================
*/

#ifndef HS_AUDIO_INCLUDED
#define HS_AUDIO_INCLUDED

#include "Gamma/Sync.h"
#include "Gamma/Gamma.h"
#include "Gamma/AudioIO.h"
#include "Gamma/Scheduler.h"

using std::cout;
using std::endl;

namespace hs{

struct Audio {

  bool bMute;
  float mMasterVolume;

  gam::AudioIO mAudioIO;
  gam::Scheduler mScheduler;

  void start(){

    #ifdef __allosphere__
		initAudio("ECHO X6", 44100, 256, 0, 60);
    #else
    cout << "DESKTOP AUDIO" << endl;
		initAudio(44100, 256);
		#endif

    mAudioIO.start();
  }

  void initAudio( double audioRate=44100, int audioBlockSize=256 );

	void initAudio(
		std::string devicename,
    double audioRate, int audioBlockSize,
		int audioInputs, int audioOutputs
	);


};

inline void Audio::initAudio(
	double audioRate, int audioBlockSize
) {

    mAudioIO.callback = gam::Scheduler::audioCB<gam::AudioIOData>;
	  mAudioIO.user(&mScheduler);

	  mAudioIO.framesPerSecond(audioRate);
	  mAudioIO.framesPerBuffer(audioBlockSize);
    gam::Sync::master().spu( mAudioIO.fps() );
}

inline void Audio::initAudio(
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

} // hs::

#endif /* end of include guard: HS_AUDIO_INCLUDED */
