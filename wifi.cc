#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/yans-wifi-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WiFi Demo");

int main(int argc, char* argv[])
{
  //first define nodes, bare devices on the network
  //NodeContainer is an easy way to manage our nodes
  NodeContainer wifiNodes;
  wifiNnodes.Create(4);

  NetDeviceContainer apNodes;
  apNodes.create(1);

  //Installing an internet stack onto the nodes so they are networked enabled (IP, TCP, UDP, etc.)
  InternetStackHelper stack;
  //The stack helper takes the node container we made above as an argument
  stack.Install(wifiNodes);
  stack.Install(apNodes);

  Ipv4AddressHelper addr;
  addr.SetBase("192.168.7.0", "255.255.255.0");
  addr.Assign(apNodes);
  addr.Assign(wifiNodes);

  /* While not used in this WiFi demo, we could init a p2p connect between 2 nodes by initing a helper and settings its data transfer rate and delay
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("100 Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("1ms"));

  Here, we would create a device container, taking our node container as an arg, and install them onto the p2p network.
  Remember this **only** works if the node container passed has 2 nodes in it
  NetDeviceContainer devices;
  devices = p2p.Install(nodes);
  */

  //Just like the p2p example, we can also manuyally set the data rate and delay, however this constructor comes with default values
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  //Init a generic layer 1 implementation
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetChannel(channel.Create());

  //Create wifi object and set the rate adaptation algorithm (think, performance)
  WifiHelper wifi;
  wifi.SetRemoteStationManager("ns3:AarfWifiManager");

  //layer 2 initialization
  WifiMacHelper mac;
  Ssid ssid = Ssid("Home Network");
  //Set device as client, set network, and toggle Active Probling bool, google for details
  mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue (false));

  //Stations or "STAs" are devices that can communicate over any 802.11 protocol (think, phone, laptop, literally everything)
  NetDeviceContainer stations;
  stations = wifi.Install(phy, mac, wifiNodes);

  //Reset mac values for access point deployment, since this is an AP, it doesnt probe
  mac.SetType("ns3:ApWifiMac", "Ssid", SsidValue(ssid));
  NetDeviceContainer aps;
  aps = wifi.Install(phy, mac, apNodes);

  //We get ready to define the mobility of the nodes, since this is a wifi network
  MobilityHelper mobility;

  //the following block inits the grid that the nodes can move around, taken from documentation for now 
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
      "MinX", DoubleValue(0.0),
      "MinY", DoubleValue(0.0),
      "DeltaX", DoubleValue(1.0),
      "DeltaY", DoubleValue(1.0),
      "GridWidth", UintegerValue(1),
      "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
      "Bounds", RectangeValue(Rectangle(-100,100 -100,100)));

  mobility.Install(wifiNodes);

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(apNodes);

  IPv4GlobalRoutingHelper::PopulateRoutingTables();

  return 0;
}
