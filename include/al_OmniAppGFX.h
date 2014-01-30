//
//  OmniAppRedux.h
//  a graphics only omni app with gui for allovsr
//
//  Created by Pablo Colapinto on 4/26/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef allovsr_OmniAppRedux_h
#define allovsr_OmniAppRedux_h

#include "allocore/al_Allocore.hpp"
#include "allocore/io/al_Window.hpp"
#include "allocore/protocol/al_OSC.hpp"
#include "alloutil/al_FPS.hpp"
#include "alloutil/al_OmniStereo.hpp"
#include "al_glv_gui.h"

#include "al_SharedData.h" 

#define PORT_TO_DEVICE_SERVER (12000)
#define PORT_FROM_DEVICE_SERVER (PORT_TO_DEVICE_SERVER+1)  

#define PORT_FROM_APP (7770)
#define PORT_TO_APP ( PORT_FROM_APP+1 )

#ifdef __allosphere__
#define DEVICE_SERVER_IP_ADDRESS "BOSSANOVA"
#endif  

#ifndef __allosphere__
#define DEVICE_SERVER_IP_ADDRESS "localhost" 
#endif

using std::cout;
using std::endl;


namespace al {


class OmniApp : public Window, public osc::PacketHandler, public FPS, public OmniStereo::Drawable {

public:
    
  
  OmniApp(std::string name = "omniapp", bool slave=false);
  virtual ~OmniApp();

  void start();
  
  virtual void onDraw(Graphics& gl) {}
  virtual void onAnimate(al_sec dt) {}   
  
  // virtual void onSound(gam::AudioIOData& io) {}
  //   
  virtual void onMessage(osc::Message& m);
  
  // const gam::AudioIO&    audioIO() const { return mAudioIO; }
  // gam::AudioIO&      audioIO(){ return mAudioIO; } 
  
  const Lens&      lens() const { return mLens; }
  Lens&        lens() { return mLens; }

  const Graphics&    graphics() const { return mGraphics; }
  Graphics&      graphics(){ return mGraphics; }
  
  const Nav&      nav() const { return mNav; }
  Nav&        nav(){ return mNav; }
  
  ShaderProgram&    shader() { return mShader; }

  const std::string&  name() const { return mName; }
  OmniApp&      name(const std::string& v){ mName=v; return *this; }

  osc::Recv&      oscRecv(){ return mOSCRecv; }
  osc::Send&      oscSend(){ return mOSCSend; }
  
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

  void initGLV();
  
  void sendHandshake();
  void sendDisconnect();
  
  virtual bool onCreate();
  virtual bool onFrame();
  virtual void onDrawOmni(OmniStereo& omni);  
  
  //  static void osend(const osc::Packet&, int port = 12001);
  
  virtual std::string  vertexCode();
  virtual std::string  fragmentCode();

protected:

  GLVGui glv;

  OmniStereo mOmni;
    
  Lens mLens;
  Graphics mGraphics;
  
  ShaderProgram mShader;
  
  // control
  Nav mNav;
  NavInputControl mNavControl;
  
  StandardWindowKeyControls mStdControls;
  osc::Recv mOSCRecv;
  osc::Send mOSCSend;
  
  std::string mName;
  std::string mHostName;
  
  double mNavSpeed, mNavTurnSpeed;
  
  bool bOmniEnable, bSlave;

};


inline OmniApp::OmniApp(std::string name, bool slave)
:  mNavControl(mNav),
  mOSCRecv(PORT_FROM_DEVICE_SERVER),
  mOSCSend(PORT_TO_DEVICE_SERVER, DEVICE_SERVER_IP_ADDRESS),
  bSlave(slave)
{  
  bOmniEnable = true;
  mHostName = Socket::hostName();
  mName = name;
  
  mNavSpeed = 1;
  mNavTurnSpeed = 0.02;
  
  lens().near(0.01).far(40).eyeSep(0.03);
  nav().smooth(0.8);
  
  Window::append(mStdControls);
  initWindow();
  initOmni();
  initGLV();
  
  if (!bSlave) {
    Window::append(mNavControl);          
    oscRecv().bufferSize(32000);
    oscRecv().handler(*this);
    sendHandshake();
  }

    //PRINT CURRENT HOST    
    printf("HOSTNAME %s\n",hostName().c_str());

    //ALLOSPHERE TEST
    //pass in _D __allosphere__ to compiler
    //by using "make <FILENAME> ALLOSPHERE = 1"
    #ifdef __allosphere__  
    
      cout << "I BELIEVE I AM RUNNING IN THE ALLOSPHERE" << endl;         
      glv.gui.disable( glv::Property::Visible );   
       
    #endif     


     #ifndef __allosphere__

      cout << "I DO NOT BELIEVE THAT I AM RUNNING IN THE ALLOSPHERE\n(if you want me to think so, make me with ALLOSPHERE = 1) " << endl;
      mOmni.mode( OmniStereo::ACTIVE ).stereo(false);     
      omniEnable(false);
      //osc = osc::Send( 7770, "localhost" );
   
     #endif

}

inline OmniApp::~OmniApp() {
  if (!bSlave) sendDisconnect();
}


inline void OmniApp::initOmni(std::string path) {
  if (path == "") {
    FILE *pipe = popen("echo ~", "r");
    if (pipe) {
      char c;
      while((c = getc(pipe)) != EOF) {
  if (c == '\r' || c == '\n')
          break;
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

inline void OmniApp::initWindow( const Window::Dim& dims, const std::string title, double fps, Window::DisplayMode mode) {
  Window::dimensions(dims);
  Window::title(title);
  Window::fps(fps);
  Window::displayMode(mode);
}

inline void OmniApp::initGLV(){
  glv.parentWindow(*this);
  glv.gui.colors().back.set(.3,.3,.3);  
  glv.gui(bOmniEnable, "omni_enable");
}

inline void OmniApp::sendHandshake(){
  oscSend().send("/handshake", name(), oscRecv().port());
}

inline void OmniApp::sendDisconnect(){
  oscSend().send("/disconnectApplication", name());
}

inline void OmniApp::start() {
  if (mOmni.activeStereo()) {
    Window::displayMode(Window::displayMode() | Window::STEREO_BUF);
  }
  
  create();
  
  if (mOmni.fullScreen()) {
    fullScreen(true);
    if (hostName() !="photon")cursorHide(true);
  }
  
  if (!bSlave) { 
    if(oscSend().opened()) sendHandshake();
  }
  
  Main::get().start();
}

inline bool OmniApp::onCreate() {
  mOmni.onCreate();
  
  Shader vert, frag;
  vert.source(OmniStereo::glsl() + vertexCode(), Shader::VERTEX).compile();
  vert.printLog();
  frag.source(fragmentCode(), Shader::FRAGMENT).compile();
  frag.printLog();
  mShader.attach
  (vert).attach(frag).link();
  mShader.printLog();
  mShader.begin();
  mShader.uniform("lighting", 0.5);
  mShader.end();
  
  return true;
}

inline bool OmniApp::onFrame() {   
  
  FPS::onFrame();

  while(oscRecv().recv()) {}
  
  //////////////
  //NAVIGATION//
  //////////////
  if (!bSlave){
    nav().step();
           
      //SEND CAMERA POSITION AND ORIENTATION TO ALL OTHER COMPUTERS (if we're in the allosphere)
      #ifdef __allosphere__
        osc::Packet p;
        p.beginMessage("/nav");
        p << nav().pos().x << nav().pos().y << nav().pos().z << nav().quat().x << nav().quat().y << nav().quat().z << nav().quat().w;
        p.endMessage();            
  
        osend(p);  
      #endif
  }  
  

  onAnimate(dt);
  
  Viewport vp(width(), height());
  
  if (bOmniEnable) {
        //cout << "enabled" << endl; 
    mOmni.onFrame(*this, lens(), nav(), vp);
  } else {
    mOmni.onFrameFront(*this, lens(), nav(), vp);
// 		double fovy = lens().fovy();
//		double aspect = vp.aspect();
//		Vec3d ux, uy, uz; 
 //   nav().unitVectors(ux, uy, uz);
		
//    mOmni.gl.projection( Matrix4d::perspective(fovy, aspect, lens().near(), lens().far()) );
//		mOmni.gl.modelView( Matrix4d::lookAt(ux, uy, uz, nav().pos()) );
 
  }
  return true;
}

inline void OmniApp::onDrawOmni(OmniStereo& omni) {
  
  graphics().error("start onDraw");
  
  mShader.begin();
  mOmni.uniforms(mShader);
  
  onDraw(graphics());
  
  mShader.end();
}


inline void OmniApp::onMessage(osc::Message& m) {
 
    if (m.addressPattern() == "/nav"){
        //std::cout << "nav" << std::endl;
        //std::cout << m.typeTags() << std::endl;  
        double x,y,z,qx,qy,qz,qw;
        m >> x; m >> y; m >> z; m >> qx; m >> qy; m >> qz; m >> qw; 
        //std::cout << vsr::Vec(x,y,z) << vsr::Quat(qx, qy, qz, qw) << endl;
        nav().pos() = al::Vec3d(x,y,z);
        nav().quat() = al::Quatd(qw, qx, qy, qz);  
      }            

}


//SHADER: 

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


#endif
