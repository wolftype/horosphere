/*
 * =====================================================================================
 *
 *       Filename:  xOSC.cpp
 *
 *    Description:  osc tests
 *
 *        Version:  1.0
 *        Created:  02/25/2014 19:53:04
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

using namespace al;
using namespace al::osc;
using namespace std;


struct MyOSC : public osc::PacketHandler {

  osc::Recv recv;
  osc::Send send;

  MyOSC() : 
    recv(7770,"localhost"), 
    send(7770,"localhost") 
  {
    recv.bufferSize(32000);
    recv.handler(*this);    
  }

  virtual void onMessage(Message& m){
    cout << m.addressPattern() << endl;
    
    float f;
    m >> f;
    
    cout << f << endl;        
  }

  virtual void bundleAndSend(float amt){
    osc::Packet packet;
    packet.beginBundle();
       packet.beginMessage("/knot/data");
       packet << amt;
       packet.endMessage();
    packet.endBundle();
    send.send(packet);
  }

};


int main(){

  MyOSC a;

  float tick = 0.0;
  while(true){
      while( a.recv.recv() ) {} 
      usleep(1000000);
      a.bundleAndSend(tick);
      tick +=.01;  
  }

  return 0;
}
