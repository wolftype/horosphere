/*
 * =====================================================================================
 *
 *       Filename:  horo_OmniApp.h
 *
 *    Description:  omni app (gfx + osc) for allosphere
 *
 *        Version:  1.0
 *        Created:  03/20/2014 15:58:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  original graham wakefiel, 
 *         modded by pablo colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  horo_OmniApp_INC
#define  horo_OmniApp_INC

//ALLOCORE
#include "allocore/al_Allocore.hpp"
#include "allocore/io/al_Window.hpp"
#include "allocore/protocol/al_OSC.hpp"

//ALLOUTIL
#include "alloutil/al_FPS.hpp"
#include "alloutil/al_OmniStereo.hpp"

//ADD-ONS
#include "horo_OSCApp.h"
#include "horo_GLVApp.h" 

using std::cout;
using std::endl;

namespace al {


class OmniApp : public Window, public OSCReceiver, public GLVApp, public FPS, public OmniStereo::Drawable {

public:
    
  OmniApp(std::string name = "omniapp", bool slave=false, bool distribute=false);
  virtual ~OmniApp();

  void start();
  
  virtual void onDraw(Graphics& gl) {}
  virtual void onAnimate(al_sec dt) {} 
  virtual void onMessage(osc::Message& m);
    
  const Lens&      lens() const { return mLens; }
  Lens&        lens() { return mLens; }

  const Graphics&    graphics() const { return mGraphics; }
  Graphics&      graphics(){ return mGraphics; }
  
  const Nav&      nav() const { return mNav; }
  Nav&        nav(){ return mNav; }
  
  ShaderProgram&    shader() { return mShader; }

  const std::string&  name() const { return mName; }
  OmniApp&      name(const std::string& v){ mName=v; return *this; }

  OmniStereo&      omni() { return mOmni; }
  
  const std::string&  hostName() const { return mHostName; }
  
  bool        omniEnable() const { return bOmniEnable; }
  void        omniEnable(bool b) { bOmniEnable = b; }
  
  void initWindow(
    const Window::Dim& dims = Window::Dim(800, 400),
    const std::string title = "OmniApp",
    double fps = 60,
    Window::DisplayMode mode = Window::DEFAULT_BUF);     
  
  void initOmni(std::string path = "");

  virtual bool onCreate();
  virtual bool onFrame();
  virtual void onDrawOmni(OmniStereo& omni);  
  
  virtual std::string  vertexCode();
  virtual std::string  fragmentCode();

  virtual bool onKeyDown(const Keyboard& k){ 
     if (k.key() == 'v') toggle();
     return true; 
  }  

protected:

  OmniStereo mOmni;
    
  Lens mLens;
  Graphics mGraphics;
  
  ShaderProgram mShader;
  
  Nav mNav;
  NavInputControl mNavControl;
  
  StandardWindowKeyControls mStdControls;

  std::string mName;
  std::string mHostName;
  
  double mNavSpeed, mNavTurnSpeed;
  
  bool bOmniEnable, bSlave, bDistribute;

};


inline OmniApp::OmniApp(std::string name, bool slave, bool distribute)
:  mNavControl(mNav),
   mNavSpeed(1),
   mNavTurnSpeed(.02),
   mName(name),
   bSlave(slave),
   bDistribute(distribute),
   GLVApp(this)
{  

  //HOST
  mHostName = Socket::hostName();
  printf("HOSTNAME %s\n",hostName().c_str());
  
  //CAMERA
  lens().near(0.01).far(40).eyeSep(0.03);
  nav().smooth(0.8);
  
  //WINDOW
  Window::append(mStdControls);
  initWindow();  
  
  //GUI
  glv.gui(bOmniEnable, "omni_enable");


  //ALLO
  #ifdef __allosphere__  
  
    cout << "I BELIEVE I AM RUNNING IN THE ALLOSPHERE" << endl;         
    
    bDistribute = true;
    omniEnable(true);
    initOmni();
    
    glv.gui.disable( glv::Property::Visible );   
  
    mOmni.mode( OmniStereo::ACTIVE ).stereo(true);     
    omniEnable(true); 
      
    if (hostName() != "gr01") {
        bSlave = true; 
        OSCReceiver::init( PORT_FROM_CLIENT_COMPUTER );
        printf("WE are GR CHILDREN\n");

    } else { 
        printf("I AM GR01********\n"); 
        OSCReceiver::init( PORT_FROM_SERVER_COMPUTER );
        bSlave = false; 
    }
     
  #endif     

  //DEBUG
  #ifndef __allosphere__

    cout << "I DO NOT BELIEVE THAT I AM RUNNING IN THE ALLOSPHERE" << endl;
    cout << "(if you want me to think so, make me with ALLOSPHERE = 1) " << endl;
    mOmni.mode( OmniStereo::ACTIVE ).stereo(false);     
    omniEnable(false);
    bSlave = false; 
    bDistribute = false;
    OSCReceiver::init( PORT_FROM_SERVER_COMPUTER );
 
  #endif

  //NAV CONTROL
  if (!bSlave) {
    Window::append(mNavControl);          
  }
}


inline OmniApp::~OmniApp() {}

/*-----------------------------------------------------------------------------
 *  GET CONFIGURATION FILES
 *-----------------------------------------------------------------------------*/
inline void OmniApp::initOmni(std::string path) {
  
  if (path == "") {
    
    FILE *pipe = popen("echo ~", "r");
    
    if (pipe) {
      
      char c;
      
      while( (c = getc(pipe) ) != EOF) {
          
          if (c == '\r' || c == '\n') break;
            
          path += c;
      }
      pclose(pipe);
    }
    path += "/calibration-current/";
  }

    //Reconfigure Projections
  mOmni.configure(path, mHostName);
  
  if (mOmni.activeStereo()) {
    cout << "active stereo = TRUE" << endl;
    mOmni.mode(OmniStereo::ACTIVE).stereo(true);
  } else{
    cout << "active stereo = FALSE" << endl;
  }
}


/*-----------------------------------------------------------------------------
 *  INITIALIZE WINDOW
 *-----------------------------------------------------------------------------*/
inline void OmniApp::initWindow( const Window::Dim& dims, const std::string title, double fps, Window::DisplayMode mode) {
  Window::dimensions(dims);
  Window::title(title);
  Window::fps(fps);
  Window::displayMode(mode);
}


inline void OmniApp::start() {
  if (mOmni.activeStereo()) {
    Window::displayMode(Window::displayMode() | Window::STEREO_BUF);
  }
  
  create(); ///< Window's create
  
  if (mOmni.fullScreen()) {
    fullScreen(true);
    if (hostName() !="photon")cursorHide(true);
  }
 
  oscRecv().start(); 
  Main::get().start();
}

inline bool OmniApp::onCreate() {
  mOmni.onCreate();
  
  Shader vert, frag;
  
  vert.source(OmniStereo::glsl() + vertexCode(), Shader::VERTEX).compile();
  vert.printLog();
  frag.source(fragmentCode(), Shader::FRAGMENT).compile();
  frag.printLog();

  mShader.attach(vert).attach(frag).link();
  mShader.printLog();
  mShader.begin();
  mShader.uniform("lighting", 0.5);
  mShader.end();
  
  return true;
}


/*-----------------------------------------------------------------------------
 *  PER FRAME
 *-----------------------------------------------------------------------------*/
inline bool OmniApp::onFrame() {   
  
  ///FRAME COUNTER
  FPS::onFrame();

  ///Listen on port from server or client
  OSCReceiver::listen();
    
  ///NAVIGATE
  if (!bSlave){

    nav().step();
    //SEND CAMERA POSITION AND ORIENTATION TO ALL OTHER COMPUTERS
    #ifdef __allosphere__
      osc::Packet p;
      p.beginMessage("/nav");
        p << nav().pos().x << nav().pos().y << nav().pos().z;
        p << nav().quat().x << nav().quat().y << nav().quat().z << nav().quat().w;
      p.endMessage();            

      SharedData::osend(p, PORT_FROM_CLIENT_COMPUTER);  
    #endif
  }  
  
  ///UPDATE STATE
  onAnimate(dt);
  
  ///RENDER GRAPHICS
  Viewport vp(width(), height());
  
  if (bOmniEnable) {
    mOmni.onFrame(*this, lens(), nav(), vp);
  } else {
    mOmni.onFrameFront(*this, lens(), nav(), vp);
  }

  return true;
}

inline void OmniApp::onDrawOmni(OmniStereo& omni) {
 
  if ( bSlave || !bDistribute) {
    graphics().error("start onDraw");
    
    mShader.begin(); 
      mOmni.uniforms(mShader);  
      onDraw(graphics());
    mShader.end();
  }

}

/*-----------------------------------------------------------------------------
 *  HANDLE NAVIGATION MESSAGES
 *-----------------------------------------------------------------------------*/
inline void OmniApp::onMessage(osc::Message& m) {

    if (bSlave){ 
      if (m.addressPattern() == "/nav"){
          double x,y,z,qx,qy,qz,qw;
          m >> x; m >> y; m >> z; m >> qx; m >> qy; m >> qz; m >> qw; 
          nav().pos() = al::Vec3d(x,y,z);
          nav().quat() = al::Quatd(qw, qx, qy, qz);  
        }            
    }
}



/*-----------------------------------------------------------------------------
 *  SHADER CODE
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


}


#endif   /* ----- #ifndef horo_OmniApp_INC  ----- */
