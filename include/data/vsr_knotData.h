/*
 * =====================================================================================
 *
 *       Filename:  vsr_knotData.h
 *
 *    Description:  data container 
 *
 *        Version:  1.0
 *        Created:  02/25/2014 13:43:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef alloprojects_knotData_h
#define alloprojects_knotData_h

#include "vsr/vsr_cga3D_op.h"

#include "allocore/protocol/al_OSC.hpp"

#include "al_SharedData.h"

using namespace al::osc;
using namespace vsr;
using namespace vsr::cga3D;

struct KnotData {

  float P, Q, vel, tube_size, writhe;

  float theta, phi, rotVel;   

  float energy, energy_scale; 
  
  float size;

  int numcables;
                 
  float diameter;
  
  bool bAutoMode;
  bool bDrawRibbon, bDrawFibers, bDrawVec, bDrawTube, bDrawPnt;     
  bool bDrawWrithe, bUseEnergies, bFlow; 
    
  vsr::Pnt pnt, pnt2; //Orbit Positions  
  vsr::Vec vec, tvec; //vector orientation for hopf fibers and target
  
  KnotData() : P(3), Q(2), vel(.01), tube_size(1), writhe(3), 
  theta(0), phi(-PIOVERFOUR), rotVel(.1), energy_scale(20),
  bAutoMode(0), vec(1,0,0), tvec(1,0,0), pnt(Ro::null(2,0,0)) {}
   
   
  /*-----------------------------------------------------------------------------
   *  MAKE OSC PACKET AND SEND  
   *-----------------------------------------------------------------------------*/
    void bundleAndSend(){
      osc::Packet knot;
  
        knot.beginBundle();
            knot.beginMessage("/knot/data");
            knot << P << Q << vel << tube_size << writhe;
            knot << pnt[0] << pnt[1] << pnt[2] << pnt[3] << pnt[4];
            knot << theta << phi << rotVel << energy_scale << size;  
            knot << int(bAutoMode) << int(bDrawRibbon) << int(bDrawFibers) << int(bDrawVec) << int(bDrawPnt); 
            knot << int(bUseEnergies) << int(bDrawWrithe) << int(bFlow) << int(bDrawTube);
            knot.endMessage();
        knot.endBundle();   
        
        #ifdef __allosphere__
        SharedData::osend( knot, PORT_FROM_DEVICE_SERVER ); //was port from app...
        #endif

        #ifndef __allosphere__
        osc::Send(PORT_FROM_DEVICE_SERVER, "localhost").send(knot);
        #endif
    
    }
    

    
    /*-----------------------------------------------------------------------------
     *  COPY DATA FROM A MESSAGE INTO LOCAL
     *-----------------------------------------------------------------------------*/
    void copyData( Message& m ){

      m >> P >> Q >> vel >> tube_size >> writhe;
      m >> pnt[0] >> pnt[1] >> pnt[2] >> pnt[3] >> pnt[4];
      m >> theta >> phi >> rotVel >> energy_scale >> size; 

      int ibAutoMode, ibDrawRibbon, ibDrawFibers, ibDrawVec, ibDrawPnt;
      int ibUseEnergies, ibDrawWrithe, ibFlow, ibDrawTube;
      
      m >> ibAutoMode >> ibDrawRibbon >> ibDrawFibers >> ibDrawVec >> ibDrawPnt;
      m >> ibUseEnergies >> ibDrawWrithe >> ibFlow >> ibDrawTube;  

      bAutoMode = ibAutoMode;
      bDrawRibbon = ibDrawRibbon;
      bDrawFibers = ibDrawFibers;
      bDrawVec = ibDrawVec;
      bDrawPnt = ibDrawPnt;
      bUseEnergies = ibUseEnergies;
      bDrawWrithe = ibDrawWrithe;
      bFlow = ibFlow;
      bDrawTube = ibDrawTube;

    }  
    
    //selective messaging from gui
    void onMessage( Message& m){
        
      if (m.addressPattern() == "/knot/data"){
          cout << "knot data received" << endl; 
          copyData(m);
      }
      else if (m.addressPattern() == "/P"){
        float f;
        m >> f;
        P = f;
      } 
      else if (m.addressPattern() == "/Q"){
        float f;
        m >> f;
        Q= f;
      } 
      else if (m.addressPattern() == "/writhe"){
        float f;
        m >> f;
        writhe= f;
      }
      else if (m.addressPattern() == "/vel"){
        float f;
        m >> f;
        vel= f; 
      } 
      else if (m.addressPattern() == "/theta"){
        float f;
        m >> f;
        theta= f;  
      }
      else if (m.addressPattern() == "/phi"){
        float f;
        m >> f;
        phi= f; 
      }
      else if (m.addressPattern() == "/bDrawFibers"){
        int f;
        m >> f;
           bDrawFibers = f;  
      }  
      else if (m.addressPattern() == "/bDrawWrithe"){
        int f;
        m >> f;
           bDrawWrithe = f;  
      }    
      else if (m.addressPattern() == "/bDrawTube"){
        int f;
        m >> f;
           bDrawTube = f; 
      } 
      else if (m.addressPattern() == "/bAutoMode"){
        int f;
        m >> f;
           bAutoMode = f; 
      } 
      else if (m.addressPattern() == "/bFlow"){
        int f;
        m >> f;
           bFlow = f; 
      }    
      else if (m.addressPattern() == "/bUseEnergies"){
        int f;
        m >> f;
           bUseEnergies = f; 
      }  
      else if (m.addressPattern() == "/energy_scale"){
        float f;
        m >> f;
           energy_scale = f; 
      }    
      else if (m.addressPattern() == "/size"){
        float f;
        m >> f;
           size = f; 
      }
    }
   
     //Built in Gui   
     template<class T>
     void buildGui(T& gui){

        gui( P, "P", 0, 100)( Q, "Q", 0, 100); 
        gui( vel, "vel",0.001,100);
        gui( tube_size, "tube_size", 0, 10);
        gui( writhe, "writhe", 1,100); 
        gui( theta, "theta", 0, PI) ( phi, "phi", -PIOVERFOUR, PIOVERFOUR );  
        gui( rotVel, "rotVel", -10,10);
        
        gui( energy_scale, "energy_scale",0,1000); 
        gui( bUseEnergies, "use_energies");
        
        gui( bFlow, "flow"); 
        gui( bAutoMode, "mode"); 
        gui( bDrawWrithe, "draw_writhe"); 
        gui( bDrawRibbon, "draw_ribbon");
        gui( bDrawFibers, "draw_fibers");
        gui( bDrawVec, "draw_vec");
        gui( bDrawTube, "draw_tube"); 
        gui( bDrawPnt, "draw_pnt"); 
        gui( size, "size", .1, 100); 
    
        //Defaults
        P = 3; Q = 2;
        pnt = vsr::Ro::null(1.5,0,0);
        vec = vsr::Vec(0,1,0);  
        vel = .015;
        writhe = 10;  
        numcables = 2;
        tube_size = 1.;  
        rotVel = .01;
      
        bDrawWrithe = true;
        bFlow = true;
     
        energy_scale = 20; 
     } 
    
};


#endif
