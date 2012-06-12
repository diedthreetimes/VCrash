#include "VehicleState.h"

// Comment this to turn off random delay
#define RANDOM_DELAY

// Padding to the packets
#define PADDING 1000
// using namespace ns3;

namespace ns3{

  uint64_t messageUID(const VehicleState::vcrash_message & msg){
    uint64_t ret = 0;
    ret = msg.broadcastId;
    ret <<= 32;
    ret |= msg.vehId;
    
    return ret;
  }

  // Generate a unique crash id
  std::string crashUID(const VehicleState::vcrash_message & msg){
    double x = msg.position_x;
    double y = msg.position_y;
    
    std::string s = "";
    s += x;
    s += ":";
    s += y;
    return s;
  }

  bool VehicleState::aggregateExists(const VehicleState::vcrash_message & msg){
    return m_crashMessages.count(crashUID(msg)) != 0;
  }
  
  // msg here could be a craash instead.
  void VehicleState::broadcast(std::string crashId, Vehicle *veh){
    vcrash_message * agr = &(veh->GetVehicleState()->m_crashMessages[ crashId ]);
    Ptr<Packet> packet = Create<Packet>((uint8_t*) agr, sizeof(vcrash_message) ); // Magic = true for serialization
    packet->AddPaddingAtEnd( PADDING );
    veh->SendTo(veh->GetBroadcastAddress(), packet);
    print_trace("Packet forwarded.",*agr,veh);
  }

  void VehicleState::receive(Vehicle * veh, ns3::Ptr<const Packet> pac, Address adr){
    vcrash_message msg;
    pac->CopyData((uint8_t *) &msg, sizeof(msg));

    bool found = false;

    // IF we have recieved the msg before
    if(m_messageHash.find(messageUID(msg)) == m_messageHash.end()){
      m_messageHash.insert(messageUID(msg));

      switch ( msg.type ){
      case MSG_VICTIM:
	if (inDistance(veh, msg.position_x, msg.position_y) == true){
	  // Create a new message to indicate that I saw an accident;
	  msg.type = MSG_SAW;
	  msg.broadcastId = this->m_broadcastId++;
	  msg.vehId = veh->GetVehicleId();
	  msg.ttl++;
	  msg.numWit = 1;
	  msg.witAddrs[0] = veh->GetVehicleId();
	  msg.vicAddrs = -1;
	}
	else{
	  msg.type = MSG_RELAY;
	  
	  if( aggregateExists(msg) ){
	    vcrash_message * agr = &(m_crashMessages[ crashUID(msg) ]);
	    if(agr->vicAddrs == -1)
	      agr->vicAddrs = msg.vicAddrs;
	  }
	}
	break;
      case MSG_SAW:
	msg.type = MSG_RELAY;

	if( aggregateExists(msg) ){
	  vcrash_message * agr = &(m_crashMessages[ crashUID(msg) ]);
	  
	  // Add the addrs if it isn't there
	  if(agr->numWit < MAGIC_NUMBER){
	    found = false;

	    for(int i=0; i < agr->numWit; i++){
	      found |= (agr->witAddrs[i] == msg.vehId);
	    }
	   
	    if(!found)
	      agr->witAddrs[agr->numWit++] = msg.vehId;
	  }
	}
	break;
      case MSG_RELAY:
	if( aggregateExists(msg) ){

	  vcrash_message * agr = &(m_crashMessages[ crashUID(msg) ]);
	  	  // Add the addrs if it isn't there
	 
	  for(int j=0; j < msg.numWit; j++) {
	    found = false;

	    for(int i=0; i < agr->numWit; i++){
	      found |= (agr->witAddrs[i] == msg.witAddrs[j]);
	    }
	    
	    if(!found && (agr->numWit < MAGIC_NUMBER))
	      agr->witAddrs[agr->numWit++] = msg.witAddrs[j];
	  }
	  if(agr->vicAddrs == -1)
	    agr->vicAddrs = msg.vicAddrs;
	}
	break;
      }
      
      // If we haven't heard about this crash yet and packet is "alive"
      if( !aggregateExists(msg) && msg.ttl > 1){
	msg.ttl--;
	std::string crashId = crashUID(msg);
	m_crashMessages[crashId]=msg;
	
	broadcast(crashId, veh);
	double delay = delayRV.GetValue();
	for(int i = 0; i < MAGIC_NUMBER; i++){
	  activeEvents.push_back(Simulator::Schedule(Seconds(i + delay + 0.5), broadcast, crashId, veh)); 
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

    double tan1 = dy/dx;
    double tan2 = tan(veh->GetDirection());
    double dtan = tan1 - tan2;
    int sign = (tan1 < tan2 ? -1 : +1);
    double theta = atan((sign * dtan) / (1 - (tan1 * tan2))) * 180 / 3.14159; // Use Math.pi;
    if (theta > 90)
      return false;


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
      msg.ttl = MAGIC_NUMBER * 2;
      msg.broadcastId = m_broadcastId++;
      msg.vehId = veh->GetVehicleId();
      msg.position_x = veh->GetPosition().x;
      msg.position_y = veh->GetPosition().y;
      msg.vicAddrs = veh->GetVehicleId();
      msg.numWit = 0;

      // Add our message to the aggregate so that we don't schedule or relay more messages
      m_crashMessages[crashUID(msg)]=msg;
    
      m_messageHash.insert(messageUID(msg));
      print_trace("Packet created.", msg, veh);
    
      Ptr<Packet> packet = Create<Packet>((uint8_t*) &msg, sizeof(vcrash_message) ); // Magic = true for serialization
      packet->AddPaddingAtEnd( PADDING );
      
      veh->SendTo(veh->GetBroadcastAddress(), packet);
      
      veh->SetVelocity(0);
      veh->SetAcceleration(0);
      veh->GetModel()->SetDesiredVelocity(0.1);
    }

  }

  void VehicleState::print_trace(string label, const vcrash_message & msg, Vehicle * veh) {
    if (trace_enabled) {
      trace_file << label <<  "$ T: " << Simulator::Now().GetNanoSeconds()
		 << ", ID: " <<  veh->GetVehicleId()
		 << ", TTL: " << msg.ttl 
		 << ", TYP: " << msg.type 
		 << ", NW: " << msg.numWit // number of witnesses
		 << ", VP: " << (msg.vicAddrs != -1)   // victims addrs exists
		 << std::endl;
    }
  }

  void VehicleState::SetTraceFile(string filename) {
    VehicleState::trace_enabled = true;
    trace_file.open(filename.c_str());
  }

  void VehicleState::CloseTraceFile() {
    trace_file.close();
  }

  int VehicleState::vehicleCrashId = 12;
  Time VehicleState::vehicleCrashTime = Seconds(30.0);
  double VehicleState::seeing_distance = 30;

  bool VehicleState::trace_enabled = false;
  std::ofstream VehicleState::trace_file;
  
  #ifdef RANDOM_DELAY
    RandomVariable VehicleState::delayRV = UniformVariable(0.0,1.0);
  #else
    RandomVariable VehicleState::delayRV = UniformVariable(0.5,0.5);
  #endif
}
