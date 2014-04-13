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

#include "allocore/protocol/al_OSC.hpp"
#include "horo_SharedData.h"

using namespace al::osc;

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
  
  float pntX, pntY, pntZ;
  float vecX, vecY, vecZ;
    
  KnotData() : P(3), Q(2), vel(.01), tube_size(1), writhe(3), 
  theta(0), phi(-3.14/4.0), rotVel(.1), energy_scale(20),
  bAutoMode(true), pntX(2), pntY(0), pntZ(0), vecX(0), vecY(1), vecZ(0) {} 
   
   
  /*-----------------------------------------------------------------------------
   *  MAKE OSC PACKET AND SEND TO ALL MACHINES  
   *-----------------------------------------------------------------------------*/
    void bundleAndSendToAll( bool TEN_G = false ){
      sendToAll( bundle(), TEN_G );
    }

    osc::Packet bundle(){
      osc::Packet knot;
  
        knot.beginBundle();
            knot.beginMessage("/knot/data");
            knot << P << Q << vel << tube_size << writhe;
            knot << pntX << pntY << pntZ;
            knot << vecX << vecY << vecZ;
            knot << theta << phi << rotVel << energy_scale << size;  
            knot << int(bAutoMode) << int(bDrawRibbon) << int(bDrawFibers) << int(bDrawVec) << int(bDrawPnt); 
            knot << int(bUseEnergies) << int(bDrawWrithe) << int(bFlow) << int(bDrawTube);
            knot.endMessage();
        knot.endBundle();   

        return knot;
     }

     /* void sendToOne(osc::Packet knot, bool TEN_G = false){ */
     /*   #ifdef __allosphere__ */
    
     /*   #endif */

     /*    #ifndef __allosphere__ */
     /*    osc::Send(PORT_FROM_DEVICE_SERVER, "localhost").send(knot); */
     /*    cout << "sending locally" << endl; */ 
     /*    #endif */

     /* } */
     
     void sendToAll(osc::Packet knot, bool TEN_G = false ) { 
        #ifdef __allosphere__
        if (TEN_G) SharedData::osend( knot, PORT_FROM_CLIENT_COMPUTER ); //was port from app...
        else SharedData::osend1G(knot, PORT_FROM_CLIENT_COMPUTER);
      //  cout << "sending data to allo network on port: " << PORT_FROM_CLIENT_COMPUTER << endl;
        #endif

        #ifndef __allosphere__
        osc::Send(PORT_FROM_SERVER_COMPUTER, "localhost").send(knot);
      //  cout << "sending locally" << endl; 
        #endif
    
    }
    

    
    /*-----------------------------------------------------------------------------
     *  COPY DATA FROM A MESSAGE INTO LOCAL
     *-----------------------------------------------------------------------------*/
    void copyData( Message& m ){

    //  cout << "receiving and copying knot data" << endl;

      m >> P >> Q >> vel >> tube_size >> writhe;
      m >> pntX >> pntY >> pntZ;
      m >> vecX >> vecY >> vecZ;

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
        float f;
        m >> f;
        bDrawFibers = f;  
      }  
      else if (m.addressPattern() == "/bDrawWrithe"){
        float f;
        m >> f;
           bDrawWrithe = f;  
      }    
      else if (m.addressPattern() == "/bDrawTube"){
        float f;
        m >> f;
           bDrawTube = f; 
      } 
      else if (m.addressPattern() == "/bAutoMode"){
        float f;
        m >> f;
           bAutoMode = f; 
      } 
      else if (m.addressPattern() == "/bFlow"){
        float f;
        m >> f;
           bFlow = f; 
      }    
      else if (m.addressPattern() == "/bUseEnergies"){
        float f;
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
        gui( theta, "theta", 0, 3.14) ( phi, "phi", -3.14/4.0, 3.14/4.0 );  
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

        gui.arrange();
    
        //Defaults
        P = 3; Q = 2;
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
