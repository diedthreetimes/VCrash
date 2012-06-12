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
    
    // IF we have recieved the msg before
    if(m_messageHash.find(messageUID(msg)) == m_messageHash.end()){
      m_messageHash.insert(messageUID(msg));
      
      if(msg.type == MSG_VICTIM){
	if (inDistance(veh, msg.position_x, msg.position_y) == true){
	  msg.type = MSG_SAW;
	  msg.ttl++;
	}
	else{
	  msg.type = MSG_RELAY;

	  // We need to work out a way to aggregate the messages here

	}
      }

      // If it hasn't gone too far yet
      if(msg.ttl > 1){
	msg.ttl--;
	
	broadcast(msg, veh);
	for(int i = 0; i < 5; i++){
	  activeEvents.push_back(Simulator::Schedule(Seconds(i + 1), broadcast, msg, veh)); 
	}
      }
      else{
	print_trace("Ignoring packet.", msg, veh);
      }
    }
  }

  bool VehicleState::inDistance(Vehicle *veh, double x, double y){
    double dx = veh->GetPosition().x - x;
    double dy = veh->GetPosition().y - y;
    if (sqrt((dx * dx) + (dy * dy)) < VehicleState::seeing_distance){
      return true;
    }
    else{
      return false;
    }
  }
  
  void VehicleState::send(Vehicle * veh) {
    if( veh->GetVehicleId()==vehicleCrashId && Simulator::Now()==vehicleCrashTime ) {
      vcrash_message msg;
      msg.type = MSG_VICTIM;
      msg.ttl = 10;
      msg.broadcastId = m_broadcastId++;
      msg.vehId = veh->GetVehicleId();
      msg.position_x = veh->GetPosition().x;
      msg.position_y = veh->GetPosition().y;
    
      m_messageHash.insert(messageUID(msg));
      print_trace("Packet created.", msg, veh);
    
      Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
      veh->SendTo(veh->GetBroadcastAddress(), packet);
      
      veh->SetVelocity(0);
      veh->SetAcceleration(0);
      veh->GetModel()->SetDesiredVelocity(0.1);
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
  double VehicleState::seeing_distance = 10;

  bool VehicleState::trace_enabled = false;
  std::ofstream VehicleState::trace_file;
}
