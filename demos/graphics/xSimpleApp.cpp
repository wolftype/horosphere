/*
 * =====================================================================================
 *
 *       Filename:  xSimpleApp.cpp
 *
 *    Description:  simple gamma + versor + omnistereo renderer
 *
 *        Version:  1.0
 *        Created:  04/04/2014 15:09:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "horo_App.h"

#include "Gamma/Oscillator.h"
#include "vsr/vsr_cga3D.h"

using namespace gam;
using namespace vsr;
using namespace vsr::cga3D;

using gam::Process;

//An Audio Process
struct SawTooth : Process {
  
  Saw<> saw;

  SawTooth() : Process(0),
  saw(440)
  {}

	void onProcess(gam::AudioIOData& io){
     while (io()) {
      float s = saw();
			io.out(0) = s;
      io.out(1) = s;
		}
	}
};


struct MyApp : App {
    
    /*-----------------------------------------------------------------------------
     *  Member Variables
     *-----------------------------------------------------------------------------*/
    Circle circleXY, circleXZ;
    float phaseA, phaseB;

    SawTooth * process;

    //Saw<> sawA;
    //Saw<> sawB;
    
    /*-----------------------------------------------------------------------------
     *  Constructor
     *-----------------------------------------------------------------------------*/
    MyApp(bool bSlave = false) : App("simple",bSlave)
    {
      init();
    }

    /*-----------------------------------------------------------------------------
     *  Destructor
     *-----------------------------------------------------------------------------*/
    virtual ~MyApp() {} 
    
    /*-----------------------------------------------------------------------------
     *  Called by Constructor
     *-----------------------------------------------------------------------------*/
    void init(){
      circleXY = CXY(1);        // Circle in XY plane
      circleXZ = CXZ(1);        // Circle in XZ plane
      phaseA = phaseB = 1;      // Phases

      process = &mScheduler.add<SawTooth>();

      glv.gui(phaseA,"phaseA",0,100); // Gui control
      glv.gui(phaseB,"phaseA",0,100);
    }
    
    /*-----------------------------------------------------------------------------
     *  Called at sound buffer rate (i.e. every 256 samples)
     *-----------------------------------------------------------------------------*/
    virtual void onSound(gam::AudioIOData& io) {
      Scheduler& s = io.user<Scheduler>();
      s.update(io);                
    }

    /*-----------------------------------------------------------------------------
     *  Called when OSC message received
     *-----------------------------------------------------------------------------*/
    virtual void onMessage(osc::Message& m) {  
        App::onMessage(m);   
    }
    
    /*-----------------------------------------------------------------------------
     *  Called Once Per Frame
     *-----------------------------------------------------------------------------*/
    virtual void onAnimate(al_sec dt){
      cout << "seconds between frames: " << dt << endl; 
      cout << "fps: " << 1/dt << endl;
    }

    /*-----------------------------------------------------------------------------
     *  Called multiple times by onFrame()
     *-----------------------------------------------------------------------------*/
    void onDraw(Graphics& g){
      
      Draw(circleXY.rot( Biv::xz * phaseA ),1,1,0);  //< Draw( object, red, green, blue ) 
      Draw(circleXZ.rot( Biv::xy * phaseB ),0,1,1);

      process -> saw.freq(80 + 240 * phaseA); 
      
    }  


};


/*-----------------------------------------------------------------------------
 *  LAUNCH APP
 *-----------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
  MyApp app; 
  app.start();  
  return 0;
}   
