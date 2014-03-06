//
//  Manages sending osc signals to all computers in allosphere network 
//  alloprojects
//
//  Created by Pablo Colapinto on 8/12/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef alloprojects_Header_h
#define alloprojects_Header_h

#include "allocore/protocol/al_OSC.hpp"

//SEND AUDIO TO 
#define PORT_TO_DEVICE_SERVER (12000)
//GET GUI INFO FROM
#define PORT_FROM_DEVICE_SERVER (PORT_TO_DEVICE_SERVER+1)  

#ifdef __allosphere__
#define DEVICE_SERVER_IP_ADDRESS "BOSSANOVA"
#define MAIN_RENDERING_MACHINE "gr01"
#endif  

#ifndef __allosphere__
#define DEVICE_SERVER_IP_ADDRESS "localhost"
#define MAIN_RENDERING_MACHINE "localhost" 
#endif

using namespace al;

struct SharedData{
    
    static void osend1G(const osc::Packet& m, int port){   
	    osc::Send(port, "gr01").send(m);    
	    osc::Send(port, "gr02").send(m); 
 	    osc::Send(port, "gr03").send(m); 
	    osc::Send(port, "gr04").send(m); 
 	    osc::Send(port, "gr05").send(m); 
	    osc::Send(port, "gr06").send(m); 
 	    osc::Send(port, "gr07").send(m); 
	    osc::Send(port, "gr08").send(m); 
 	    osc::Send(port, "gr09").send(m); 
	    osc::Send(port, "gr10").send(m); 
	    osc::Send(port, "gr11").send(m); 
	    osc::Send(port, "gr12").send(m); 
	    osc::Send(port, "gr13").send(m); 

 }

  static void osend(const osc::Packet& m, int port){   
	    osc::Send(port, "192.168.80").send(m);    
	    osc::Send(port, "192.168.81").send(m); 
 	    osc::Send(port, "192.168.82").send(m); 
	    osc::Send(port, "192.168.83").send(m); 
 	    osc::Send(port, "192.168.84").send(m); 
	    osc::Send(port, "192.168.85").send(m); 
 	    osc::Send(port, "192.168.86").send(m); 
	    osc::Send(port, "192.168.87").send(m); 
 	    osc::Send(port, "192.168.88").send(m); 
	    osc::Send(port, "192.168.89").send(m); 
	    osc::Send(port, "192.168.90").send(m); 
	    osc::Send(port, "192.168.91").send(m); 
	    osc::Send(port, "192.168.92").send(m); 
	    osc::Send(port, "192.168.93").send(m); 
}

};



#endif


/* 192.168.0.15" */
/* 192.168.0.46" */
/* 192.168.0.47" */
/* 192.168.0.48" */
/* 192.168.0.49" */
/* 192.168.0.50" */
/* 192.168.0.51" */
/* 192.168.0.53" */
/* 192.168.0.54" */
/* 192.168.0.55" */
/* 192.168.0.56" */
/* 192.168.0.71" */
