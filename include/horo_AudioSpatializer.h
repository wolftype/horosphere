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

using namespace gfx;  
using namespace std;

using gfx::Vec3f;
using gfx::Quat;
   
#define ALLO_SPEAKER_LAYOUT 60

template< int N >
struct Speakers {
        
	  int num() { return N; }   		
  
	  Speakers(float r = 3) { calcRolloff(r); init(); }

    gfx::Vec3f pos[N]; 
		float RollOff; 

		void init();

		vector<float>  mix( gfx::Vec3f v ); //dbap at v
				
		//Group of channels
		struct Channel {
			vector<int> idx; 		
		};


		void calcRolloff( float dec) { RollOff = pow(10., -dec/20.0); }

		int move( int ch, int n ){   		
			return (ch + n) % N;
		}
		int opp( int ch ) { return move ( ch, N / 2. ); } 
		int next( int ch ){ return move (ch, ch + 1); }	
		int reflect( int ch)  { return ref[ch]; } 

		int ref[N];
	
		static vector<Channel> Group;

	};
	
	template<int N>
	inline vector<float> Speakers<N> :: mix  (gfx::Vec3f v) {

		vector<float> m;
		
		float sum;
		for (int i = 0; i < N; ++i){
			gfx::Vec3f dir = pos[i] - v;
			
			float mag = dir.sq();
			float id = 0;
			float dist;
			if (!mag == 0 ){
				id =1.0 / mag;
				dist = 1.0/sqrt(mag);
			}
			m.push_back( dist );
			sum += id;   					
		}

		//cout << RollOff << " RO " << endl;
		float k = RollOff / sqrt(sum);

		for (int i = 0; i < N; ++i){
			m[i] *= k; 
		}  
		
		// //zero out non-existant speakers
		// for (int i = 13; i < 16; ++i){
		// 	
		// }  
		
		return m;
		
	}   
	
	template<> 
	inline void Speakers<2> :: init(){
		pos[0] = gfx::Vec3f(-1,0,0);
		pos[1] = gfx::Vec3f(1,0,0);
	}         
	
	template<>
	inline void Speakers<ALLO_SPEAKER_LAYOUT> :: init()   {
		 
		gfx::Vec3f mv(-1,0,0);

		//1 - 12 top ring 
	    int ix = 0;  
		gfx::Vec3f tv = gfx::Quat::spin( mv, gfx::Quat( PIOVERFOUR / 2.0, gfx::Vec3f(0,0,-1) ) );  
		cout << "TOP " << tv << endl; 
		 for (int i = 0; i < 12; ++i){   
			float t = 1.0 * ix / 12.0;
			gfx::Quat q( PI * t, gfx::Vec3f(0,1,0));
			pos[i] = gfx::Quat::spin( tv, q ); 
			cout << i << " " << pos[i] << endl;
			ix++;
		}  


		//17 - 46 middle ring
	   ix = 0;   
		cout << "middle " << mv << endl; 
		for (int i = 16; i < 46; ++i){  
			float t = 1.0 * ix / 20.0;
			gfx::Quat q( PI * t, gfx::Vec3f(0,1,0));
			pos[i] = gfx::Quat::spin( mv, q ); 
			ix++;
		}
		
		//49-60 bottom ring 
		ix = 0;  
		gfx::Vec3f bv = gfx::Quat::spin( mv, gfx::Quat( -PIOVERFOUR/2.0, gfx::Vec3f(0,0,-1) ) ); 
			   cout << "bottom " << bv << endl; 
		for (int i = 48; i < 60; ++i){  
			double t = 1.0 * ix / 12.0;
			gfx::Quat q( PI * t, gfx::Vec3f(0,1,0));
			pos[i] = gfx::Quat::spin( bv, q ); 
			ix++;
		}    	
		//note: subwoofer is [47]
	}


#endif
