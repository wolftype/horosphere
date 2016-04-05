/*
 * =====================================================================================
 *
 *       Filename:  horo_AudioSpatializer.h
 *
 *    Description:  speaker layout spatializing experiments
 *
 *        Version:  1.0
 *        Created:  02/26/2014 16:06:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:
 *
 * =====================================================================================
 */


#ifndef MY_AUDIO_SPATIALIZER_INCLUDED
#define MY_AUDIO_SPATIALIZER_INCLUDED

#include "gfx/gfx_matrix.h"
#include <vector>

using gfx::Vec3f;
using gfx::Quat;

namespace hs {

  #define DESKTOP_SPEAKER_LAYOUT 0
  #define NUM_DESKTOP_SPEAKERS 2

  #define ALLO_SPEAKER_LAYOUT 1
  #define NUM_ALLO_SPEAKERS 60




  /// Layout of Spatialized Speakers, templated on LAYOUT and NUMBER
  template<int L, int N >
  struct SpeakerLayout {

  	SpeakerLayout(float r = 3) { calcRolloff(r); init(); }

    /// Initialize Geometry (DEFAULTS TO DESKTOP)
    void init(){

      cout << "INIT DESKTOP SPEAKER LAYOUT" << endl;

  		pos[0] = gfx::Vec3f(-1,0,0);
  		pos[1] = gfx::Vec3f(1,0,0);
  	}

    /// Audio Mix at position
    struct Mix {

      SpeakerLayout * sl;

      int numChannels(){ return N; }

      Mix (){
        sl = new SpeakerLayout();//add rolloff?
        channel.reserve(N);
      }

      /// mix in each channel
      vector<float> channel;//= vector<float> channel(N);//[N];
    	gfx::Vec3f pos;

    	float operator[] (int idx) const{
    		return channel[idx];
    	}

      Mix& operator()(){
        return sl -> mix(*this);
      }
    };

    /// Positions of Speakers
    gfx::Vec3f pos[N];
    float RollOff;

    /// Calculate DBAP at v
    Mix& mix( Mix& m );
    /// Calculate ROLLOFF
  	void calcRolloff( float dec) { RollOff = pow(10., -dec/20.0); }
    /// Number of Channels
  	int num() { return N; }

    /// Move by n channels
  	int move( int ch, int n ){
  		return (ch + n) % N;
  	  }

    /// Select opposite channel
  	int opp( int ch ) { return move ( ch, N / 2. ); }
    /// Select to next channel
  	int next( int ch ){ return move (ch, ch + 1); }
    /// Select reflected channel
  	int reflect( int ch)  { return ref[ch]; }
  	int ref[N];

  };

  /// fill N floats at f with DBAP at v
	template<int L, int N>
	inline typename SpeakerLayout<L,N>::Mix& SpeakerLayout<L,N> :: mix  ( SpeakerLayout<L,N>::Mix& mx ) {

		float sum;
		for (int i = 0; i < N; ++i){
			gfx::Vec3f dir = pos[i] - mx.pos;

			float mag = dir.sq();
			float id = 0;
			float dist;
			if (!(mag == 0) ){
				id =1.0 / mag;
				dist = 1.0/sqrt(mag);
			}
			mx.channel[i]= dist;
			sum += id;
		}

		float k = RollOff / sqrt(sum);

		for (int i = 0; i < N; ++i){
			mx.channel[i] *= k;
		}

    return mx;

	}

  /// ALLOSPHERE SPEAKER LAYOUT note: subwoofer is [47]
	template<>
	inline void SpeakerLayout<ALLO_SPEAKER_LAYOUT, NUM_ALLO_SPEAKERS> :: init()   {

    cout << "INIT ALLOSPHERE SPEAKER LAYOUT" << endl;

		gfx::Vec3f mv(-1,0,0);

		//1 - 12 top ring
	  int ix = 0;
		gfx::Vec3f tv = gfx::Quat::spin( mv, gfx::Quat( PIOVERFOUR / 2.0, gfx::Vec3f(0,0,-1) ) );
		//cout << "TOP " << tv << endl;
		 for (int i = 0; i < 12; ++i){
			float t = 1.0 * ix / 12.0;
			gfx::Quat q( PI * t, gfx::Vec3f(0,1,0));
			pos[i] = gfx::Quat::spin( tv, q );
			//cout << i << " " << pos[i] << endl;
			ix++;
		}


		//17 - 46 middle ring
	  ix = 0;
		//cout << "middle " << mv << endl;
		for (int i = 16; i < 46; ++i){
			float t = 1.0 * ix / 20.0;
			gfx::Quat q( PI * t, gfx::Vec3f(0,1,0));
			pos[i] = gfx::Quat::spin( mv, q );
			ix++;
		}

		//49-60 bottom ring
		ix = 0;
		gfx::Vec3f bv = gfx::Quat::spin( mv, gfx::Quat( -PIOVERFOUR/2.0, gfx::Vec3f(0,0,-1) ) );
		//cout << "bottom " << bv << endl;
		for (int i = 48; i < 60; ++i){
			double t = 1.0 * ix / 12.0;
			gfx::Quat q( PI * t, gfx::Vec3f(0,1,0));
			pos[i] = gfx::Quat::spin( bv, q );
			ix++;
		}

	}

  using DesktopSpeakers = SpeakerLayout<DESKTOP_SPEAKER_LAYOUT, NUM_DESKTOP_SPEAKERS>;
  using AlloSpeakers = SpeakerLayout<ALLO_SPEAKER_LAYOUT, NUM_ALLO_SPEAKERS>;
  using DesktopMix = SpeakerLayout<DESKTOP_SPEAKER_LAYOUT, NUM_DESKTOP_SPEAKERS>::Mix;
  using AlloMix = SpeakerLayout<ALLO_SPEAKER_LAYOUT, NUM_ALLO_SPEAKERS>::Mix;

} //hs::


#endif
