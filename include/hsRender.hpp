#ifndef HS_RENDER_INCLUDED
#define HS_RENDER_INCLUDED

#include "gfx/gfx_gl.h"

#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "hsVsr.hpp"

namespace hs {

  template<class T>
  struct RenderApp : al::OmniStereoGraphicsRenderer {

      cuttlebone::Taker<typename UserData<T>::State> taker;
      UserData<T> mUser;

      /*-----------------------------------------------------------------------------
       *  Start
       *-----------------------------------------------------------------------------*/
      virtual void start() {
        
        mUser.bind();
        taker.start();                        // non-blocking
        al::OmniStereoGraphicsRenderer::omni().resolution(2048);
        al::OmniStereoGraphicsRenderer::start();  // blocks
      }

      /*-----------------------------------------------------------------------------
       *  Get State
       *-----------------------------------------------------------------------------*/
      virtual bool onFrame(){
        auto& s = mUser.mState.mData;
        al::OmniStereoGraphicsRenderer::lens().eyeSep( s.eyesep ).near(s.near);
        auto b = al::OmniStereoGraphicsRenderer::onFrame();
        int popCount = taker.get(mUser.mState);
        return b;
      }

      /*-----------------------------------------------------------------------------
       *  Draw Loop Called Multiple Times Per Frame
       *-----------------------------------------------------------------------------*/
       virtual void onDraw(al::Graphics& g){
         auto& s = mUser.mState.mSceneData;
         //MODEL pose to axis angle
         glPushMatrix();
           gfx::Vec4<> tr = s.model.quat().axan();
           glRotatef ( tr[3], tr[0], tr[1], tr[2]  );

           mUser.onDraw();

        glPopMatrix();
       }

       /*-----------------------------------------------------------------------------
        *  Physics Called Once per Frame (do something with state)
        *-----------------------------------------------------------------------------*/
       virtual void onAnimate(al_sec dt){
          auto& s = mUser.mState.mSceneData;

          pose = gfx2al::pose( s.camera ); //conversion to al world
          mUser.updateLocal();

       }

  };

} //hs::


#endif /* end of include guard: HS_RENDER_INCLUDED */
