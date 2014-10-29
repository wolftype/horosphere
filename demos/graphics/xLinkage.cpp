/*
 * =====================================================================================
 *
 *       Filename:  xLinkage.cpp
 *
 *    Description:  Bennett Mechanisms
 *
 *        Version:  1.0
 *        Created:  10/28/2014 11:59:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  AlloSphere Research Group 
 *
 * =====================================================================================
 */

#include "horo_OmniApp.h"   
#include "horo_GxSync.h"

#include "vsr/vsr_cga3D_op.h"
#include "vsr/vsr_cga3D_draw.h"

#include "vsr_linkages.h"
#include "vsr_chain_draw.h"
#include "gfx/gfx_mesh.h"


using namespace al;
using namespace vsr;
using namespace vsr::cga3D; 

using std::cout;
using std::endl;
  
struct MyApp : OmniApp {

 // Light light;

  float time;
  float numiter;          //number of iterations to sweep single instance
  float numx,numy;
  float d1,d2;            //root linkage lengths
  float theta;            //skew of root linkage
  float thetax,thetay;    //skew of subsequent linkage, in terms of pecentage of previous
  float rot;              //phase of mechanism
  float amtx1,amtx2;      //percentage along length of xcoord link a and b
  float amty1,amty2;      //percentage along length of ycoord link a and b
  float link;             //which link to link with
  bool bSubBennett;

  vector<Bennett> bennett;
  vector<vector<Bennett>> subBennett;

    //bS flag = true if machine is "receiver" machine
    MyApp(bool bS = false) :        
      OmniApp("vortex",bS), 
      time(0)
    {
      init();
      initGui();
    }     

    virtual ~MyApp() {}
  
    void initGui(){
      glv.gui(numiter,"numiter",1,100);
      glv.gui(numx,"numx",0,100)(numy,"numy",0,100);
      glv.gui(d1,"d1",0,100);
      glv.gui(d2,"d2",0,100);
      glv.gui(theta,"theta",-TWOPI,TWOPI);

      glv.gui(thetax,"thetax",-TWOPI,TWOPI);
      glv.gui(thetay,"thetay",-TWOPI,TWOPI);

      glv.gui(rot,"rot",-100,100);

      glv.gui(amtx1,"amtx1",-1,1);
      glv.gui(amtx2,"amtx2",-1,1);
      glv.gui(amty1,"amty1",-1,1);
      glv.gui(amty2,"amty2",-1,1);

      glv.gui(link,"link",0,3);
      glv.gui(bSubBennett,"subbennett");

      numx=1; 
      link = 2;
      d1 = 3; d2 = 3; theta = PIOVERFOUR; rot = .5; amtx1=amtx2=amty1=amty2=.5; thetax = -1; thetay = -1;
  }
    
    void init();
    void step();
  
    virtual void onAnimate(al_sec dt);
    virtual void onDraw(Graphics& g);

    virtual void updateState();               ///< Called OnFrame
    virtual void sendData();                  ///< Send Data to Other Instances of App (Called if Dist Mode)
    virtual void sendAudioData();             ///< Send Data To Audio App
    virtual void onMessage(osc::Message& m);  ///< Handle incoming Messages  
   
};

/*-----------------------------------------------------------------------------
 *  INITIALIZATION
 *-----------------------------------------------------------------------------*/
void MyApp::init(){

}

/*-----------------------------------------------------------------------------
 *  CALLED ONCE PER FRAME (Called by OmniApp::onFrame() )
 *-----------------------------------------------------------------------------*/
void MyApp::onAnimate(al_sec dt){


    step();  
    updateState();

    //Send Data if Master      
    if (!bSlave){ 
      sendData();
      sendAudioData(); 
    } 
    
}

/*-----------------------------------------------------------------------------
 * UPDATE STATE (MASTER) //Only the MASTER computer does this
 *-----------------------------------------------------------------------------*/
void MyApp::updateState(){
    //clear
    bennett.clear(); subBennett.clear();
    //allocate
    bennett = vector<Bennett>(numx);
    if (bSubBennett) subBennett = vector<vector<Bennett>>(numx/2.0);

    bennett[0].set(theta,d1,d2);
    bennett[0]( rot );
    bool bSwitch =true; int it=0;
    for (int i=1;i<numx;++i){
      bennett[i]=bennett[i-1].linkAt(2,thetax,amtx1,amtx2);

      bSwitch = !bSwitch;
      if (bSwitch){ if (bSubBennett) {
        subBennett[it] = vector<Bennett>(numy);
        
        subBennett[it][0] = bennett[i].linkAt(1,thetay,amty1,amty2); 
        for (int j=1;j<numy;++j){
          subBennett[it][j]=subBennett[it][j-1].linkAt(2,thetay,amty1,amty2);
        }
        it++;
      }}

    }


}

/*-----------------------------------------------------------------------------
 *  CALLED MANY TIMES PER FRAME: Draw all objects 
 *-----------------------------------------------------------------------------*/
void MyApp::onDraw(Graphics& g) {
    
    gfx::GL::lightPos(1,1,1);
    gfx::GL::light();
    
    for (int i=0;i<numx;++i){
      Draw( (Chain)bennett[i], true, false, .5,.5,.5);
      DrawR( (Chain)bennett[i], .5,.5,.5);
    }

    for (auto& i : subBennett){ 
      for (auto& j: i) {
        Draw( (Chain)j, true, false, .5,.5,.5);
        DrawR( (Chain)j, .5,.5,.5);
      } 
    }
}


/*-----------------------------------------------------------------------------
 *  SEND UPDATED DATA
 *-----------------------------------------------------------------------------*/
void MyApp::sendData(){

}

/*-----------------------------------------------------------------------------
 *  Send Audio Data To Audio App
 *-----------------------------------------------------------------------------*/
void MyApp::sendAudioData(){

}

/*-----------------------------------------------------------------------------
 *  Get Messages
 *-----------------------------------------------------------------------------*/
void MyApp :: onMessage(osc::Message& m) {   
    if (bSlave){  
      OmniApp::onMessage(m);
    } 
    
    if(bDistribute){
        
    }
}

void MyApp::step(){

}


/*-----------------------------------------------------------------------------
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  MyApp app; 
  app.start();  
  return 0;
}       
       

