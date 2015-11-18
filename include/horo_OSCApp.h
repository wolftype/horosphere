/*
 * =====================================================================================
 *
 *       Filename:  horo_OSCApp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/18/2014 19:30:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

//#include "allocore/al_Allocore.hpp"
#include "allocore/protocol/al_OSC.hpp"
#include "horo_SharedData.h"
#include <iostream>

using namespace std;

struct OSCReceiver : public osc::PacketHandler {
  
  osc::Recv *mOSCRecv;
  osc::Recv& oscRecv(){ return *mOSCRecv; }

  OSCReceiver( int port = 0 )  {
    if (port!=0) init(port);
  }

  virtual void init(int port){
    mOSCRecv = new osc::Recv(port);
    oscRecv().bufferSize(32000);
		oscRecv().handler(*this);
    oscRecv().timeout(.01);
  }
  
  virtual void start(){
    oscRecv().start();
  }

  virtual void listen(){
    while(oscRecv().recv()){}
  }

  virtual void onMessage(osc::Message& m){
    cout << "Received message: " <<  m.addressPattern() << endl;
  } 

};

struct OSCSender {

	osc::Send mOSCSend;  
  
  OSCSender(int port, string dest) :
  mOSCSend(port, dest.c_str()){}


};


struct OSCApp : public osc::PacketHandler  {   

	osc::Recv mOSCRecv;
	osc::Send mOSCSend;  

	osc::Recv&			oscRecv(){ return mOSCRecv; }
	osc::Send&			oscSend(){ return mOSCSend; }    
  
  OSCApp(int in, int out, string dest) :
  mOSCRecv(in),
	mOSCSend(out, dest.c_str())    
  {
    //mOSCRecv = new osc::Recv(in)
    #ifdef __allosphere__
      cout << "INSIDE ALLOSPHERE" << endl;
    #endif

    init();
  } 

  virtual void init(){
    initOSC();
    //initWindow();
  }
	
  virtual void initOSC(){
    oscRecv().bufferSize(32000);
		oscRecv().handler(*this);
    oscRecv().timeout(.01);
  }

  virtual void listen(){
    while(oscRecv().recv()){}
  }

  virtual void onMessage(osc::Message& m){
    cout << "OSC APP message: " <<  m.addressPattern() << endl;
  }  

	void start(){   
		mOSCRecv.start();
  //  MainLoop::start();
	} 

    //if also a window . . .
    
    /* virtual bool onFrame(){ */
    
  /*   glClearColor(0,0,0,1); */
  /*   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */
    
  /*   listen(); */ 
     
  /*   update(); */
  /*   onDraw(); */ 
    
  /*   return true; */
  /* } */

    /* void initWindow(){ */
  /*   create( Window::Dim(800, 800), "OSCApp", 60, Window::DEFAULT_BUF ); */
  /* } */

  
  /* virtual void update(){} */ 
  /* virtual void onDraw(){} */
};
