/*
 * =====================================================================================
 *
 *       Filename:  xOSCApp.cpp
 *
 *    Description:  osc send / receive test
 *
 *        Version:  1.0
 *        Created:  03/18/2014 13:20:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "allocore/al_Allocore.hpp"
#include "allocore/protocol/al_OSC.hpp"
#include "al_SharedData.h"
#include <iostream>

using namespace std;

struct OSCApp : public al::Window, public osc::PacketHandler  {   

	osc::Recv mOSCRecv;
	osc::Send mOSCSend;  

	osc::Recv&			oscRecv(){ return mOSCRecv; }
	osc::Send&			oscSend(){ return mOSCSend; }    
  
  OSCApp() :
  mOSCRecv(PORT_FROM_DEVICE_SERVER),
	mOSCSend(PORT_FROM_DEVICE_SERVER, MAIN_RENDERING_MACHINE)    
  {
  
    initOSC();
    initWindow();

    #ifdef __allosphere__
      cout << "INSIDE ALLOSPHERE" << endl;
    #endif
  } 
	
  void initWindow(){
    create( Window::Dim(800, 800), "OSCApp", 60, Window::DEFAULT_BUF );
  }

  virtual void initOSC(){
    oscRecv().bufferSize(32000);
		oscRecv().handler(*this);
  }

  virtual bool onFrame(){
    
    glClearColor(0,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    while(oscRecv().recv()) {}  
 
    SharedData::SendToMain( SharedData::TestPacket() );
     
    update();
    onDraw(); 
    
    return true;
  }

  virtual void update(){} 
  virtual void onDraw(){}

  virtual void onMessage(osc::Message& m){
    cout << "OSC APP message: " <<  m.addressPattern() << endl;
  }  

	void start(){   
		mOSCRecv.start();
    MainLoop::start();
	} 
};

OSCApp app;

int main(){
  SharedData::print();
  app.start();
}
