/*
 * =====================================================================================
 *
 *       Filename:  vsr_audioData.h
 *
 *    Description:  audio data messaging
 *
 *        Version:  1.0
 *        Created:  02/25/2014 19:17:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */



#ifndef alloprojects_audioData_h
#define alloprojects_audioData_h

#include "al_SharedData.h" 

struct AudioData{

  float freq, px, py, pz, px2,py2,pz2, width, nrg, nftheta, nfphi, fwrithe, ratio, ftheta, fphi; 

    void bundleAndSend(){
        
      osc::Packet knot;  
    
      knot.beginBundle();
       
      knot.beginMessage("/event/freq");
      knot << freq ;
      knot.endMessage();  
      
      knot.beginMessage("/wind/x");
      knot << float(px) ;
      knot.endMessage();  
      
      knot.beginMessage("/wind/y");
      knot << float(py) ;
      knot.endMessage();
      
      knot.beginMessage("/wind/z");
      knot << float(pz) ;
      knot.endMessage();
 
      knot.beginMessage("/wind/freq");
      knot << freq ;
      knot.endMessage();  
      
      knot.beginMessage("/wind/width");
      knot << width;
      knot.endMessage();
      
      knot.beginMessage("/fm/lfreq") ;
      knot << nrg * 10;
      knot.endMessage();  
      
      knot.beginMessage("/fm/c1") ;
      knot << nftheta;
      knot.endMessage();  
      
      knot.beginMessage("/fm/c2") ;
      knot << nfphi;
      knot.endMessage(); 
      
      knot.beginMessage("/harmonics/lfreq") ;
      knot << fwrithe;
      knot.endMessage(); 
      
      knot.beginMessage("/harmonics/cdelay") ;
      knot << ratio;
      knot.endMessage();
                         
      knot.beginMessage("/harmonics/cffd") ;
      knot << ftheta;
      knot.endMessage();  
      
      knot.beginMessage("/harmonics/lmod") ;
      knot << fphi;
      knot.endMessage();  
      
      knot.beginMessage("/harmonics/x");
      knot << float(px2) ;
      knot.endMessage();  
      
      knot.beginMessage("/harmonics/y");
      knot << float(py2) ;
      knot.endMessage();
      
      knot.beginMessage("/harmonics/z");
      knot << float(pz2) ;
      knot.endMessage();    
      
    knot.endBundle(); 
     
    #ifndef __allosphere__
    osc::Send(PORT_TO_DEVICE_SERVER, DEVICE_SERVER_IP_ADDRESS).send(knot); 
    #endif

    #ifdef __allosphere__
    osc::Send(PORT_TO_DEVICE_SERVER, DEVICE_SERVER_IP_ADDRESS).send(knot);
    #endif

    }

     //Built in Gui   
     template<class T>
     void buildGui(T& glv){};


};


#endif
