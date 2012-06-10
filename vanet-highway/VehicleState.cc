#include "VehicleState.h"

namespace ns3 {
  void VehicleState::receive(Vehicle * veh, Ptr<const Packet> pac, Address adr){
        vcrash_message msg;
      pac->CopyData((uint8_t *) &msg, sizeof(msg));
  
      if(msg.ttl > 1){
	msg.ttl--;
	printf("Packet forwarded. TTL: %i, ID: %i\n", msg.ttl, veh->GetVehicleId());
	Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
	veh->SendTo(veh->GetBroadcastAddress(), packet);
      }
  }

  void VehicleState::send(Vehicle * veh) {
      if( veh->GetVehicleId()==vehicleCrashId && Simulator::Now()==vehicleCrashTime ) {
	/*stringstream msg;
	  msg << "I hate you so so much: ";
	  msg << Simulator::Now().GetMicroSeconds() << std::endl;


	  std::cout << msg.str() << std::endl;
	  Ptr<Packet> packet = Create<Packet>((uint8_t*) msg.str().c_str(), msg.str().length());
	*/
	vcrash_message msg;
	msg.ttl = 3;
    
	printf("Packet created. TTL: %i, ID: %i\n", msg.ttl, veh->GetVehicleId());

	Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
	veh->SendTo(veh->GetBroadcastAddress(), packet);
      }

    }

  int VehicleState::vehicleCrashId = 10;
  Time VehicleState::vehicleCrashTime= Seconds(20.0);

}
