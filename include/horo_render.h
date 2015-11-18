/*
 * =====================================================================================
 *
 *       Filename:  horo_render.h
 *
 *    Description:  rendering in the allosphere
 *
 *        Version:  1.0
 *        Created:  05/22/2015 16:44:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef  horo_render_INC
#define  horo_render_INC


#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"


template<class TState>
struct OmniRenderBone : al::OmniStereoGraphicsRenderer {

    cuttlebone::Taker<TState> taker;
    TState* state;
    
    /*-----------------------------------------------------------------------------
     *  Start
     *-----------------------------------------------------------------------------*/
    virtual void start() {
      state = new TState;
      memset(state, 0, sizeof(TState));
      taker.start();                        // non-blocking
      al::OmniStereoGraphicsRenderer::start();  // blocks
    }

    /*-----------------------------------------------------------------------------
     *  Get State
     *-----------------------------------------------------------------------------*/
    virtual bool onFrame(){
      auto b = al::OmniStereoGraphicsRenderer::onFrame();
      int popCount = taker.get(*state);
      return b;
    }

};


#endif   /* ----- #ifndef horo_render_INC  ----- */
