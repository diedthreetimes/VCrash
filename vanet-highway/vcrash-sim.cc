/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005-2009 Old Dominion University [ARBABI]
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Hadi Arbabi <marbabi@cs.odu.edu>
 */

/*
  This the starting point of the simulation and experiments.
  The main function will parse the input and parameter settings.
  Creates a highway and set the highway parameters. then bind the events (callbacks)
  to the created controller and designed handlers. Sets the highway start and end time,
  and eventually runs the simulation which is basically running a highway with a controller.
  You can add your functions to controller to create various scenarios.
*/

#include <fstream>
#include <iostream>
#include <iomanip>
#include "Model.h"
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/random-variable.h"
#include "math.h"
#include "Highway.h"
#include "VehicleGenerator.h"
#include "tinyxml.h"
#include "HighwayProjectXml.h"
#include "HighwayXml.h"
#include "WifiConfigurationXml.h"
#include "HighwayProject.h"
#include "VehicleState.h"

NS_LOG_COMPONENT_DEFINE("VCrash");

using namespace ns3;
using namespace std;

/*static void addCustomVehicle(Ptr<Highway> highway) {
  int id = -1;
  int lane = 1;
  double speed = 40.0;

  Ptr<Model> model = CreateObject<Model>();
  model->SetDesiredVelocity(speed);
  model->SetDeltaV(4.0);
  model->SetAcceleration(0.5);  
  model->SetDeceleration(3.0);  
  model->SetMinimumGap(2.0);
  model->SetTimeHeadway(0.1);
  model->SetSqrtAccelerationDeceleration(sqrt(model->GetAcceleration() * model->GetDeceleration()));

  Ptr<LaneChange> laneChange = CreateObject<LaneChange>();
  laneChange->SetPolitenessFactor(0.2);
  laneChange->SetDbThreshold(0.3);
  laneChange->SetGapMin(2.0);
  laneChange->SetMaxSafeBreakingDeceleration(12.0);
  laneChange->SetBiasRight(0.2);

  Ptr<Vehicle> temp=CreateObject<Vehicle>();
  temp->SetVehicleId(id);
  temp->IsEquipped=false;
  temp->SetModel(model);
  temp->SetLaneChange(laneChange);
  temp->SetLength(4);
  temp->SetWidth(2);
  temp->SetVelocity(speed);
  temp->SetAcceleration(0.0);
  temp->SetLane(lane);
  temp->SetDirection(0.0);
  temp->SetPosition(highway->GetLaneStart(lane));
  highway->AddVehicleToBeginning(temp);
  }
*/
//static int msgCounter = 0;

static void vehicleReceive(Ptr<Vehicle> veh, Ptr<const Packet> pac, Address adr){
  veh->GetVehicleState()->receive(&(*veh),pac,adr);  
}

static bool controlVehicle(Ptr<Highway> highway, Ptr<Vehicle> veh, double dt) {
  veh->GetVehicleState()->send(&(*veh));
  return false;
}

int main(int argc, char *argv[]) {

  LogComponentEnable ("VCrash", LOG_LEVEL_INFO);
  
  string projectXmlFile = "";
  string vehicleTraceFile = "vehicleTrace.csv";
  string networkTraceFile = "networkTrace.csv";
  bool enableVehicleReceive = false;
  bool enableDeviceTrace = false;
  bool enablePhyRxOkTrace = false;
  bool enablePhyRxErrorTrace = false;
  bool enablePhyTxTrace = false;
  bool enablePhyStateTrace = false;

  CommandLine cmd;
  cmd.AddValue("project", "highway xml description", projectXmlFile);
  cmd.AddValue("vehtracefile", "trace file for vehicle locations", vehicleTraceFile);
  cmd.AddValue("nettracefile", "trace file for network messages", networkTraceFile);
  cmd.AddValue("enablevehiclereceive", "enable tracing vehicle receive", enableVehicleReceive);
  cmd.AddValue("enabledevicetrace", "enable device trace", enableDeviceTrace);
  cmd.AddValue("enablephyrxoktrace", "enable phy rx ok trace", enablePhyRxOkTrace);
  cmd.AddValue("enablephyrxerrortrace", "enable phy rx error trace", enablePhyRxErrorTrace);
  cmd.AddValue("enablephytxtrace", "enable phy tx trace", enablePhyTxTrace);
  cmd.AddValue("enablephystatetrace", "enable phy state trace", enablePhyStateTrace);

  cmd.Parse(argc, argv);

  // Parse the highway file
  TiXmlDocument doc(projectXmlFile.c_str());
  doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* root = hDoc.FirstChildElement().Element();
  TiXmlHandle hroot = TiXmlHandle(root);
  HighwayProjectXml xml;
  xml.LoadFromXml(hroot);

  // Configure Wifi
  ns3::PacketMetadata::Enable();
  Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));

  // Load config options
  HighwayProject project(xml);
  project.SetVehTraceFile(vehicleTraceFile);
  //project.SetNetTraceFile(networkTraceFile); //project traces aren't very useful...
  if(enableVehicleReceive) {
    project.SetVehicleReceiveCallback(MakeCallback(&vehicleReceive)); 
    project.EnableVehicleReceive();
  }
  if(enableDeviceTrace) {
    project.EnableDeviceTrace();
  }
  if(enablePhyRxOkTrace) {
    project.EnablePhyRxOkTrace();
  }
  if(enablePhyRxErrorTrace) {
    project.EnablePhyRxErrorTrace();
  }
  if(enablePhyTxTrace) {
    project.EnablePhyTxTrace();
  }
  if(enablePhyStateTrace) {
    project.EnablePhyStateTrace();
  }
    
  // Configure a vehicle control
  Ptr<Highway> highway = project.getHighways()[0];
  //Simulator::Schedule(Seconds(10), &addCustomVehicle, highway);
  project.SetVehicleControlCallback(MakeCallback(&controlVehicle));
  
  VehicleState::SetTraceFile(networkTraceFile);

  project.Start();

  Simulator::Run();
  Simulator::Destroy();

  VehicleState::CloseTraceFile();

  return 0;
}
