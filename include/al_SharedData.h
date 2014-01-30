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

using namespace al;

struct SharedData{
    
    static void osend(const osc::Packet& m, int port = 12001){
        osc::Send(port, "192.168.0.15").send(m);  //photon   
	    osc::Send(port, "192.168.0.46").send(m);  //gr01   
	    osc::Send(port, "192.168.0.47").send(m);  //gr04
 	    osc::Send(port, "192.168.0.48").send(m);  //gr05
	    osc::Send(port, "192.168.0.49").send(m);  //gr06
 	    osc::Send(port, "192.168.0.50").send(m);  //gr07
	    osc::Send(port, "192.168.0.51").send(m);  //gr08
 	    osc::Send(port, "192.168.0.53").send(m);  //gr09
	    osc::Send(port, "192.168.0.54").send(m);  //gr10
 	    osc::Send(port, "192.168.0.55").send(m);  //gr11
	    osc::Send(port, "192.168.0.56").send(m); //gr12 
	    osc::Send(port, "192.168.0.71").send(m); //gr13
}

};



#endif
