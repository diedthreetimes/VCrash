#ifndef CLASS_VEHICLE_STATE_
#define CLASS_VEHICLE_STATE_


#include "ns3/core-module.h"
#include "ns3/ptr.h"
#include "ns3/address.h"

namespace ns3 {
  class Vehicle;

  class VehicleState{
  public:
    static int vehicleCrashId;
    static Time vehicleCrashTime;

    struct vcrash_message {
      int ttl;
    };// __attribute__(packed) ;
    

    void receive(Vehicle * veh, Ptr<const Packet> pac, Address adr);
    void send(Vehicle *veh);
  };
  
}
#endif
