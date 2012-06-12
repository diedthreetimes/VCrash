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
  
  void VehicleState::broadcast(vcrash_message msg, Vehicle *veh){
    Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
    veh->SendTo(veh->GetBroadcastAddress(), packet);
    print_trace("Packet forwarded.",msg,veh);
  }

  void VehicleState::receive(Vehicle * veh, ns3::Ptr<const Packet> pac, Address adr){
    vcrash_message msg;
    pac->CopyData((uint8_t *) &msg, sizeof(msg));
  
    if(m_messageHash.find(messageUID(msg)) == m_messageHash.end()){
      m_messageHash.insert(messageUID(msg));
      if(msg.ttl > 1){
	msg.ttl--;

	broadcast(msg, veh);
	for(int i = 0; i < 5; i++){
	  activeEvents.push_back(Simulator::Schedule(Seconds(i + 1), broadcast, msg, veh)); 
	}
      }
    }
    else{
      print_trace("Ignoring packet.", msg, veh);
    }
  }

  void VehicleState::send(Vehicle * veh) {
    if( veh->GetVehicleId()==vehicleCrashId && Simulator::Now()==vehicleCrashTime ) {
      vcrash_message msg;
      msg.ttl = 10;
      msg.broadcastId = m_broadcastId++;
      msg.vehId = veh->GetVehicleId();
    
      m_messageHash.insert(messageUID(msg));
      print_trace("Packet created.", msg, veh);

      Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
      veh->SendTo(veh->GetBroadcastAddress(), packet);
    }

  }

  void VehicleState::print_trace(string label, vcrash_message msg, Vehicle * veh) {
    if (trace_enabled) {
      trace_file << label <<  "$ T: " << Simulator::Now().GetNanoSeconds()
		 << ", ID: " <<  veh->GetVehicleId()
		 << ", TTL: " << msg.ttl <<std::endl;
    }
  }

  void VehicleState::SetTraceFile(string filename) {
    VehicleState::trace_enabled = true;
    trace_file.open(filename.c_str());
  }

  void VehicleState::CloseTraceFile() {
    trace_file.close();
  }

  int VehicleState::vehicleCrashId = 4;
  Time VehicleState::vehicleCrashTime = Seconds(15.0);
  bool VehicleState::trace_enabled = false;
  std::ofstream VehicleState::trace_file;
}
