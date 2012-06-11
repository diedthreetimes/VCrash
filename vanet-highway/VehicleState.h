#ifndef CLASS_VEHICLE_STATE_
#define CLASS_VEHICLE_STATE_


#include "ns3/core-module.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/packet.h"
#include <set>

namespace ns3 {

  class Vehicle;

  class VehicleState{
    
  private:
    int m_broadcastId;
    std::set<uint64_t> m_messageHash;
    
  public:
    static int vehicleCrashId;
    static Time vehicleCrashTime;

    struct vcrash_message {
      int ttl;
      int broadcastId;
      int vehId;
    };// __attribute__(packed) ;
    
    VehicleState(){
      m_broadcastId = 0;
    }

    void receive(Vehicle * veh, Ptr<const Packet> pac, Address adr);
    void send(Vehicle *veh);
  };
  
}

#include "Vehicle.h"
#endif
