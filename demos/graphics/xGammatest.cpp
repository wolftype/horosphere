/*
 * =====================================================================================
 *
 *       Filename:  xGammatest.cpp
 *
 *    Description:  gamma test
 *
 *        Version:  1.0
 *        Created:  02/26/2014 17:47:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>				// for printing to stdout
#include "Gamma/Gamma.h"		// core functions
#include "Gamma/Access.h"
#include "Gamma/AudioIO.h"
#include "Gamma/Delay.h"
#include "Gamma/DFT.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/FormantData.h"
#include "Gamma/Noise.h"
#include "Gamma/Oscillator.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/Print.h"
#include "Gamma/Recorder.h"
#include "Gamma/Scheduler.h"
#include "Gamma/SoundFile.h"
#include "Gamma/Timer.h"
#include "Gamma/Types.h"
#include "Gamma/UnitMaps.h"

using namespace gam;

Accum<> tmr(1./2);
NoiseWhite<> white;		// 1/f^0 noise
NoisePink<> pink;		// 1/f^1 noise
NoiseBrown<> brown;		// 1/f^2 noise
int type = 2;			// Noise type


void audioCB(AudioIOData& io){

	while(io()){
	
		if(tmr()) (++type)%=3;
		
		float s = 0;
		
		switch(type){
			case 0: s = white()*0.4;break;
			case 1: s = pink();		break;
			case 2: s = brown();	break;
		}

		io.sum(s*0.2, 0,1);
	}
}


int main(int argc, char* argv[]){
	AudioIO io(256, 44100, audioCB, NULL, 2);
	Sync::master().spu(io.framesPerSecond());
	io.start();
	printf("Press 'enter' to quit...\n"); getchar();
	return 0;
}
