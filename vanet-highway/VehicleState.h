#ifndef CLASS_VEHICLE_STATE_
#define CLASS_VEHICLE_STATE_

#include "ns3/core-module.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/packet.h"
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "ns3/random-variable.h"
#include <math.h>

// number of witness addresses in the message
#define MAGIC_NUMBER 5

namespace ns3 {

  class Vehicle;

  class VehicleState{

  public:
    static int vehicleCrashId;
    static Time vehicleCrashTime;
    static double seeing_distance;
    static RandomVariable delayRV;
    // Message Types
    static const int MSG_VICTIM = 1;
    static const int MSG_SAW = 2;
    static const int MSG_RELAY = 3;

    struct vcrash_message {
      int type;
      int ttl;
      int broadcastId;
      int vehId;
      double position_x;
      double position_y;
      
      // Aggregates
      int witAddrs[MAGIC_NUMBER];
      int numWit; // the number of witnesses
      int vicAddrs; // victims address
    };
    
    VehicleState(){
      m_broadcastId = 0;
    }
    ~VehicleState(){
       for(unsigned int i=0; i < activeEvents.size(); i++)
	 Simulator::Cancel(activeEvents.at(i));
    }
    
    void receive(Vehicle * veh, Ptr<const Packet> pac, Address adr);
    bool inDistance(Vehicle *veh, double x, double y);
    void send(Vehicle *veh);
    static void broadcast(std::string crashId, Vehicle *veh);
    static void SetTraceFile(std::string filename);
    static void CloseTraceFile();

  private:
    int m_broadcastId;
    std::set<uint64_t> m_messageHash;
    std::map<std::string,vcrash_message> m_crashMessages;
    std::vector<EventId> activeEvents;

    bool aggregateExists(const vcrash_message &);

    static std::ofstream trace_file;
    static bool trace_enabled;
    static void print_trace(std::string label, const vcrash_message & msg, Vehicle * veh);
  };
  
}

#include "Vehicle.h"
#endif
