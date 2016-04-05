#ifndef HS_RENDER_INCLUDED
#define HS_RENDER_INCLUDED

#include "gfx/gfx_gl.h"

#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include "horo_vsr.hpp"

namespace hs {

  template<class T>
  struct RenderApp : al::OmniStereoGraphicsRenderer {

      cuttlebone::Taker<typename T::State> taker;
      T mParam;

      /*-----------------------------------------------------------------------------
       *  Start
       *-----------------------------------------------------------------------------*/
      virtual void start() {
        taker.start();                        // non-blocking
        al::OmniStereoGraphicsRenderer::start();  // blocks
      }

      /*-----------------------------------------------------------------------------
       *  Get State
       *-----------------------------------------------------------------------------*/
      virtual bool onFrame(){
        auto b = al::OmniStereoGraphicsRenderer::onFrame();
        int popCount = taker.get(mParam.mState);
        return b;
      }

      /*-----------------------------------------------------------------------------
       *  Draw Loop Called Multiple Times Per Frame
       *-----------------------------------------------------------------------------*/
       virtual void onDraw(al::Graphics& g){
         auto& s = mParam.mState.mScene;
         //MODEL pose to axis angle
         glPushMatrix();
           gfx::Vec4<> tr = s.model.quat().axan();
           glRotatef ( tr[3], tr[0], tr[1], tr[2]  );

           mParam.onDraw();

        glPopMatrix();
       }

       /*-----------------------------------------------------------------------------
        *  Physics Called Once per Frame (do something with state)
        *-----------------------------------------------------------------------------*/
       virtual void onAnimate(al_sec dt){
          auto& s = mParam.mState.mScene;
          //cout << s.time << endl;
          pose = gfx2al::pose( s.camera ); //conversion to al world

          mParam.updateLocal();

       }

  };

} //hs::


#endif /* end of include guard: HS_RENDER_INCLUDED */
