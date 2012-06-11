#include "VehicleState.h"

// using namespace ns3;

namespace ns3 {

  uint64_t messageUID(const VehicleState::vcrash_message & msg){
    uint64_t ret = 0;
    ret = msg.broadcastId;
    ret <<= 32;
    ret |= msg.vehId;
    
    return ret;
  }
  void VehicleState::receive(Vehicle * veh, ns3::Ptr<const Packet> pac, Address adr){
        vcrash_message msg;
      pac->CopyData((uint8_t *) &msg, sizeof(msg));
  
      if(m_messageHash.find(messageUID(msg)) == m_messageHash.end()){
	m_messageHash.insert(messageUID(msg));
	if(msg.ttl > 1){
	  msg.ttl--;
	  printf("Packet forwarded. TTL: %i, ID: %i, T: %ld\n", msg.ttl, veh->GetVehicleId(), Simulator::Now().GetNanoSeconds());
	  Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
	  veh->SendTo(veh->GetBroadcastAddress(), packet);
	}
      }
      else{
	printf("I'm ignoring a packet %i.\n", veh->GetVehicleId());
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
	msg.ttl = 10;
	msg.broadcastId = m_broadcastId++;
	msg.vehId = veh->GetVehicleId();
    
	m_messageHash.insert(messageUID(msg));
	printf("Packet created. TTL: %i, ID: %i, T: %ld\n", msg.ttl, veh->GetVehicleId(), Simulator::Now().GetNanoSeconds());

	Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
	veh->SendTo(veh->GetBroadcastAddress(), packet);
      }

    }

  int VehicleState::vehicleCrashId = 4;
  Time VehicleState::vehicleCrashTime= Seconds(32.0);

}
