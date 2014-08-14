/*
 * =====================================================================================
 *
 *       Filename:  alloMarchingCubes.cpp
 *
 *    Description:  port of CUDA sdk marching cubes demo to allosphere
 *
 *        Version:  1.0
 *        Created:  08/13/2014 14:50:08
 *       Revision:  none
 *       Compiler:  gcc (and nvcc)
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization: allosphere research group 
 *
 * =====================================================================================
 */


#include "marchingCubes_x.h"
#include "horo_OmniApp.h"

using namespace al;

using std::cout;
using std::endl;

struct App : OmniApp {

    MarchingCubesProgram mc;

    App(int argc, char ** argv, bool bS = false) :  
      
      OmniApp("vortex",bS),
      mc(argc,argv)

    {
      init(); 
    }     

    virtual ~App() {}
   
    /*-----------------------------------------------------------------------------
     *  INITIALIZATION
     *-----------------------------------------------------------------------------*/
    void init(){
    }


    void sendData();
    void step();
  
    virtual void onDraw(Graphics& g);

    virtual void onAnimate(al_sec dt);

    virtual void onMessage(osc::Message& m);  

    virtual bool onCreate(){
      OmniApp::onCreate();

       //Now that a context exists, we can initialize glew
       glewExperimental = true;     // This flag helps prevent errors with modern OpenGL
       GLenum glewError = glewInit();
       if (glewError != GLEW_OK){
         printf("glew init error\n%s\n", glewGetErrorString( glewError) );
       }
      
      mc.run();
      return true;
    }

};


void App::sendData(){}

void App::onAnimate(al_sec dt){
  
    if (!bSlave){     
      sendData(); 
    } 
  
    step(); 
   
    mc.isoValue += mc.dIsoValue;
		if (mc.isoValue < 0.1f ) {
      mc.isoValue = 0.1f;
			mc.dIsoValue *= -1.0f;
		} else if( mc.isoValue > 0.9f ) {
			mc.isoValue = 0.9f;
			mc.dIsoValue *= -1.0f;
		}
    
    mc.computeIsosurface();     
}


void App :: onMessage(osc::Message& m) {   
    if (bSlave){  
      OmniApp::onMessage(m);
    }  
}

void App::step(){

}

void App::onDraw(Graphics& g) {
  mc.onDraw();
  /* glBegin(GL_LINE_STRIP); */
  /*   glVertex3f(-1,0,0); */
  /*   glVertex3f(1,0,0); */
  /*   glVertex3f(0,1,0); */
  /* glEnd(); */
}




int main(int argc, char * argv[]) {

  App app(argc,argv); 
  
  app.start();  

  return 0;
}       
       
