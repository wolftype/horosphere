//  Created by Pablo Colapinto on 2/26/13.

#include "horo_OmniApp.h"   
#include "horo_GxSync.h"

#include "vsr/vsr_cga3D_op.h"
#include "vsr/vsr_cga3D_draw.h"

using namespace al;
using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;
  
struct MyApp : OmniApp {


    //osc::Send osc;   //send data to audio
    //osc::Recv oscR;  //get from gui
    //

    /* TIME BASED VARIABLES */
    float time;

    Circle cir;

    //bS flag = true if machine is "receiver" machine
    MyApp(bool bS = false) :  
      
      OmniApp("vortex",bS), 
     // oscR(7771),
      time(0)
    {
        
      //oscR.bufferSize(32000);
      //oscR.handler(*this); 
      
      init();
      
    }     

    virtual ~MyApp() {}

    
    /*-----------------------------------------------------------------------------
     *  INITIALIZATION
     *-----------------------------------------------------------------------------*/
    void init(){
      cir = CXY(1);
    }


    void sendData();
    void step();
  
    virtual void onDraw(Graphics& g);

    virtual void onAnimate(al_sec dt);

    virtual void onMessage(osc::Message& m);  
    
    virtual bool onMouseDown(const Mouse& m ){
      cout << m.x() << " " << m.y() <<endl;
      return true;
    }

    /* virtual bool onKeyDown(const Keyboard& k){ */ 
    
    /*    if (k.key() == 'v') glv.gui.toggle( glv::Property::Visible ); */
    /*    return true; */ 
    /* } */  
   
};


void MyApp::sendData(){}

void MyApp::onAnimate(al_sec dt){
  
//    while(oscR.recv()) {}     //get from gui  
      
    if (!bSlave){     
      sendData(); 
    } 
  
    step();      
}


void MyApp :: onMessage(osc::Message& m) {   
    if (bSlave){  
      OmniApp::onMessage(m);
    }  
}

void MyApp::step(){

}

void MyApp::onDraw(Graphics& g) {

  // Draw all objects to scene
  Draw(cir);
  
}




int main(int argc, char * argv[]) {
  
  MyApp app; 
  
  app.start();  

  return 0;
}       
       
