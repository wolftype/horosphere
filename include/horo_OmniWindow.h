/*
 * =====================================================================================
 *
 *       Filename:  horo_OmniWindow.h
 *
 *    Description:  just windowing of omnistereo renderer
 *
 *        Version:  1.0
 *        Created:  09/03/2014 17:35:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "allocore/al_Allocore.hpp"
#include "allocore/io/al_Window.hpp" 
#include "alloutil/al_OmniStereo.hpp"

namespace al{

struct Omni : public Window, public OmniStereo::Drawable {

  /*-----------------------------------------------------------------------------
   *  GRAPHICS CONTEXT
   *-----------------------------------------------------------------------------*/
  Graphics mGraphics;
  const Graphics&    graphics() const { return mGraphics; }
  Graphics&      graphics(){ return mGraphics; } 
  
  /*-----------------------------------------------------------------------------
   * RENDERER  
   *-----------------------------------------------------------------------------*/
  OmniStereo mOmni;
  OmniStereo&      omni() { return mOmni; }  
    
  /*-----------------------------------------------------------------------------
   *  SCENE SHADER
   *-----------------------------------------------------------------------------*/
  ShaderProgram mShader;
  ShaderProgram&    shader() { return mShader; }   
  
  /*-----------------------------------------------------------------------------
   *  PERSPECTIVE
   *-----------------------------------------------------------------------------*/
  Lens mLens;
  const Lens&      lens() const { return mLens; }
  Lens&        lens() { return mLens; }  

  /*-----------------------------------------------------------------------------
   *  POSITION
   *-----------------------------------------------------------------------------*/
  Nav mNav; 
  const Nav&      nav() const { return mNav; }
  Nav&        nav(){ return mNav; }  

  /*-----------------------------------------------------------------------------
   *  FLAGS
   *-----------------------------------------------------------------------------*/
  bool bOmniEnable;

  Omni(std::string name = "omni");
  virtual ~Omni();

  void init(  const Window::Dim& dims = Window::Dim(800, 400),
              const std::string title = "OmniApp",
              double fps = 60,
              Window::DisplayMode mode = Window::DEFAULT_BUF)
  {
    Window::dimensions(dims);
    Window::title(title);
    Window::fps(fps);
    Window::displayMode(mode);
  }

  virtual bool onCreate(){

    //Configure Projections
    mOmni.configure("", mHostName); 

    //Build various omni shaders
    mOmni.onCreate();

    //Build default scene shader
    compileShader(vertexcode(), fragmentcode());

  }

  virtual bool onFrame(){

    ///UPDATE STATE
    onAnimate(dt);
  
    ///RENDER GRAPHICS
    Viewport vp(width(), height());

    if (bOmniEnable) {
      mOmni.onFrame(*this, lens(), nav(), vp);
    } else {
      mOmni.onFrameFront(*this, lens(), nav(), vp);
    }   
  }

  virtual void compileShader(std::string vertsrc, std::string fragsrc){
    //compile shader
    Shader vert, frag;

    vert.source( OmniStereo::glsl() + vertsrc, Shader::VERTEX).compile();
    frag.source(fragsrc, Shader::FRAGMENT).compile();

    mShader.attach(vert).attach(frag).link();

  }

  
  /*-----------------------------------------------------------------------------
   *  Get Configuration Files 
   *-----------------------------------------------------------------------------*/
  void initOmniStereo(std::string path);

  virtual void onDrawOmni(OmniStereo& omni);

}

inline void OmniApp::onDrawOmni(OmniStereo& omni) {
 
  if ( bSlave || !bDistribute) {
   // graphics().error("start onDraw");
    
    mShader.begin(); 
      mOmni.uniforms(mShader);  
      onDraw(graphics());
    mShader.end();
  }

}


inline void OmniWindow::configureOmniStereo(std::string path){
  //Configure Projections
  mOmni.configure(path, mHostName);  
  if (mOmni.activeStereo()) {
    cout << "active stereo = TRUE" << endl;
    mOmni.mode(OmniStereo::ACTIVE).stereo(true);
  } else{
    cout << "active stereo = FALSE" << endl;
  }  
}


/*-----------------------------------------------------------------------------
 *  DEFAULT SHADER CODE
 *-----------------------------------------------------------------------------*/
inline std::string  OmniApp::vertexCode() {
  return AL_STRINGIFY(
    varying vec4 color;
    varying vec3 normal, lightDir, eyeVec;
    void main(){
      color = gl_Color;
      vec4 vertex = gl_ModelViewMatrix * gl_Vertex;
      normal = gl_NormalMatrix * gl_Normal;
      vec3 V = vertex.xyz;
      eyeVec = normalize(-V);
      lightDir = normalize(vec3(gl_LightSource[0].position.xyz - V));
      gl_Position = omni_render(vertex); 
    }
  );
}

inline std::string OmniApp::fragmentCode() {
  return AL_STRINGIFY(
    uniform float lighting;
    varying vec4 color;
    varying vec3 normal, lightDir, eyeVec;
      void main() {
      vec4 final_color = color * gl_LightSource[0].ambient;
      vec3 N = normalize(normal);
      vec3 L = lightDir;
      float lambertTerm = max(dot(N, L), 0.0);
      final_color += gl_LightSource[0].diffuse * color * lambertTerm;
      vec3 E = eyeVec;
      vec3 R = reflect(-L, N);
      float spec = pow(max(dot(R, E), 0.0), 0.9 + 1e-20);
      final_color += gl_LightSource[0].specular * spec;
      gl_FragColor = color;//mix(color, final_color, lighting); //color;//
    }
  );
} 
