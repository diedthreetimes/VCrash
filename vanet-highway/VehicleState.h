#ifndef CLASS_VEHICLE_STATE_
#define CLASS_VEHICLE_STATE_

#include "ns3/core-module.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/packet.h"
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

namespace ns3 {

  class Vehicle;

  class VehicleState{

  public:
    static int vehicleCrashId;
    static Time vehicleCrashTime;
    static double seeing_distance;

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
    };// __attribute__(packed) ;
    
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
    static void broadcast(vcrash_message msg, Vehicle *veh);
    static void SetTraceFile(std::string filename);
    static void CloseTraceFile();

  private:
    int m_broadcastId;
    std::set<uint64_t> m_messageHash;
    pthread_t broadcast_thread;
    std::vector<EventId> activeEvents;

    static std::ofstream trace_file;
    static bool trace_enabled;
    static void print_trace(std::string label, vcrash_message msg, Vehicle * veh);
  };
  
}

#include "Vehicle.h"
#endif
