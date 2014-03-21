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


#ifdef __allosphere__
#define SERVING_MACHINE "BOSSANOVA"
#define MAIN_RENDERING_MACHINE "gr01"
//GUI LISTENS ON THIS PORT
#define PORT_FROM_INTERFACE (12000)
//GET GUI INFO FROM (MASTER COMPUTER LISTENS ON THIS PORT)
#define PORT_FROM_SERVER_COMPUTER (12001) 
//SEND MASTER INFO (CLUSTER LISTTENS ON THIS PORT, as does AUDIO COMPUTER)
#define PORT_FROM_CLIENT_COMPUTER (12002)
#endif  

//LOCAL HOST SEND/RECEIVE TO SAME PORT
#ifndef __allosphere__
#define SERVING_MACHINE "localhost"
#define MAIN_RENDERING_MACHINE "localhost" 
//GUI LISTENS ON THIS PORT
#define PORT_FROM_INTERFACE (12000)
//GET GUI INFO FROM (MASTER COMPUTER LISTENS ON THIS PORT)
#define PORT_FROM_SERVER_COMPUTER (12001) 
//SEND MASTER INFO (CLUSTER LISTTENS ON THIS PORT, as does AUDIO COMPUTER)
#define PORT_FROM_CLIENT_COMPUTER (12002)
#endif

#include <iostream>

using namespace al;
using namespace std;

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
      osc::Send(port, "gr14").send(m); 
 }

  static void osend(const osc::Packet& m, int port){   
	    osc::Send(port, "192.168.10.80").send(m);    
	    osc::Send(port, "192.168.10.81").send(m); 
 	    osc::Send(port, "192.168.10.82").send(m); 
	    osc::Send(port, "192.168.10.83").send(m); 
 	    osc::Send(port, "192.168.10.84").send(m); 
	    osc::Send(port, "192.168.10.85").send(m); 
 	    osc::Send(port, "192.168.10.86").send(m); 
	    osc::Send(port, "192.168.10.87").send(m); 
 	    osc::Send(port, "192.168.10.88").send(m); 
	    osc::Send(port, "192.168.10.89").send(m); 
	    osc::Send(port, "192.168.10.90").send(m); 
	    osc::Send(port, "192.168.10.91").send(m); 
	    osc::Send(port, "192.168.10.92").send(m); 
	    osc::Send(port, "192.168.10.93").send(m); 
}


    static osc::Packet TestPacket(float f = 1.0){
      osc::Packet test;
        test.beginBundle();
          test.beginMessage("/test");
            test << f;
          test.endMessage();
        test.endBundle();
      return test;
    }

    static void SendToAll(const osc::Packet& m, bool TEN_G = false ) { 
       
        #ifdef __allosphere__
        if (TEN_G) SharedData::osend( m, PORT_FROM_CLIENT_COMPUTER ); //was port from app...
        else SharedData::osend1G(m, PORT_FROM_CLIENT_COMPUTER);
        cout << "sending in allo network on port: " << PORT_FROM_CLIENT_COMPUTER << endl;
        #endif

        #ifndef __allosphere__
        osc::Send(PORT_FROM_SERVER_COMPUTER, "localhost").send(m);
        cout << "sending locally" << endl; 
        #endif
    
    }

    static void SendToMainFromServer(const osc::Packet& m ){
//        cout << "sending to main" << endl; 
        osc::Send(PORT_FROM_SERVER_COMPUTER, MAIN_RENDERING_MACHINE).send(m);
    }

    static void SendToServerFromClient(const osc::Packet& m ){
        osc::Send(PORT_FROM_CLIENT_COMPUTER, SERVING_MACHINE).send(m);
    }


    static void print(){
      std::cout << "MAIN RENDERING MACHINE: " << MAIN_RENDERING_MACHINE << std::endl;
      std::cout << "PORT_FROM_INTERFACE: " << PORT_FROM_INTERFACE << std::endl; 
      std::cout << "PORT_FROM_SERVER_COMPUTER: " << PORT_FROM_SERVER_COMPUTER << std::endl; 
      std::cout << "PORT_FROM_CLIENT_COMPUTER: " << PORT_FROM_CLIENT_COMPUTER << std::endl; 

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
