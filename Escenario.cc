
#include "ns3/command-line.h"
#include "ns3/integer.h"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/node-container.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/udp-client.h"
#include "ns3/udp-server.h"
#include "ns3/data-rate.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/point-to-point-channel.h"
#include "ns3/csma-helper.h"
#include "ns3/bridge-helper.h"

#include "ns3/udp-client-server-helper.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/queue-disc-container.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/on-off-helper.h"
#include "observador.h"

using namespace ns3;

//Componente de trazado como proyecto
NS_LOG_COMPONENT_DEFINE("Proyecto");

Ptr<Node> PuenteHelper (NodeContainer nodosLan, NetDeviceContainer & d_nodosLan,DataRateValue c_transmision);
void escenario(Time duracion_simulacion,Time intervalo_video,Time intervalo_texto,DataRateValue c_transmision,uint16_t port_servidor);

int main (int arcg, char *argv[])
{

  Time::SetResolution(Time::NS);

  Time duracion = Time("60s");
  Time intervalo = Time("1s");	
  std::string intervalo_video_value="100us";
  std::string intervalo_texto_value="50us";
  std::string regBin="100kbps";
  int tamPaq_video = 1024;
  int tamPaq_texto = 820;
  int tam_cola=10000;
  uint16_t port_servidor=20;

  //Línea de comandos
  CommandLine cmd;
  cmd.AddValue("duracion","duración de la simulación",duracion);
  cmd.AddValue("intervalo_video","tiempo entre dos paquetes consecutivos de formto video",intervalo_video_value);
  cmd.AddValue("intervalo_texto","tiempo entre dos paquetes consecutivos de formato texto", intervalo_texto_value);
  cmd.AddValue("regBin", "Tasa de envio de los dispositivos", regBin);	
  cmd.AddValue("tamPaq_video","tamaño medio de los paquetes de videos en octetos ",tamPaq_video);
  cmd.AddValue("tamPaq_texto","tamaño medio de los paquetes de texto en octetos ",tamPaq_texto);
  cmd.AddValue("tam_cola","tamaños de la cola del servidor en octetos",tam_cola);
  cmd.Parse (arcg,argv);


  Time intervalo_video =Time(intervalo_video_value);
  Time intervalo_texto =Time(intervalo_texto_value);

  DataRateValue RegBin = DataRateValue(regBin);
  
  escenario(duracion,intervalo_video,intervalo_texto,RegBin,port_servidor);
  


}

void escenario(Time duracion_simulacion,Time intervalo_video, Time intervalo_texto,DataRateValue c_transmision,uint16_t port_servidor){
  //Definimos el contenedor de nodos de universidades
  NodeContainer c_univ;
  //Creamos tantos nodos como facultades
  c_univ.Create(4);
  
  Names::Add ("rectorado",c_univ.Get(0));
  Names::Add ("viapol",c_univ.Get(1));
  Names::Add ("reina_mercedes",c_univ.Get(2));
  Names::Add ("cartuja",c_univ.Get(3));
  
  //Creamos nodos de servidores
  NodeContainer c_serv;
  c_serv.Create(1);
  
  Names::Add ("servidor",c_serv.Get(0));//hacemos un nombrado a los nodos para posteriormente poder buscarlo mas sencillo
  
  //Ptr<CsmaNetDevice> servidor_device=Names::Find<CsmaNetDevice> ("servidor");

  //Creamos un contenedor donde meter todos los nodos
  NodeContainer c_todos;
  c_todos.Add (c_serv);
  c_todos.Add(c_univ);
  
  NetDeviceContainer c_dispositivos;

  
  //Añadimos la pila
  InternetStackHelper h_pila; 
  h_pila.SetIpv6StackInstall (false); 
  h_pila.SetIpv4ArpJitter (true);
  h_pila.Install (c_todos);

  //Utilizamos PuenteHelper para conectar los servidores y los clientes.
  Ptr<Node> puente = PuenteHelper(c_todos, c_dispositivos,c_transmision);

  //Añadimos la cola al servidor vamos a tener dos cola 

  TrafficControlHelper tch;
  uint16_t handle = tch.SetRootQueueDisc ("ns3::PrioQueueDisc", "Priomap", StringValue ("0 1 0 1 0 1 0 1 0 1 0 1 0 1 0 1"));
  TrafficControlHelper::ClassIdList cid = tch.AddQueueDiscClasses (handle, 2, "ns3::QueueDiscClass");
  tch.AddChildQueueDisc (handle, cid[0], "ns3::FifoQueueDisc");
  tch.AddChildQueueDisc (handle, cid[1], "ns3::FifoQueueDisc");
  
  QueueDiscContainer queue_container = tch.Install(c_dispositivos.Get(0));
  

  //Asignamos direcciones IPv4

  Ipv4AddressHelper h_direcciones ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer c_interfaces =h_direcciones.Assign (c_dispositivos);
  //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  NS_LOG_DEBUG("dir ip del servidor es "<< c_interfaces.GetAddress(0,0));
  NS_LOG_DEBUG("dir ip del nodos 1 es "<< c_interfaces.GetAddress(1,0));
  NS_LOG_DEBUG("dir ip del nodo 2 es "<< c_interfaces.GetAddress(2,0));
  NS_LOG_DEBUG("dir ip del nodo 3 es "<< c_interfaces.GetAddress(3,0));
  
  //Configuramos multicasting

  Ipv4Address multicastSource("10.1.1.1");
  Ipv4Address multicastGroup("225.1.2.4");

  Ipv4StaticRoutingHelper multicast;

  //establecemos una ruta estatica en el emisor el servidor
  Ptr<Node> sender=c_todos.Get(0);
  Ptr<NetDevice> senderif=c_dispositivos.Get(0);
  multicast.SetDefaultMulticastRoute(sender,senderif);

  //creamos dos aplicaciones de OnOff en el servidor una para trafico de video y otra para texto
  
  uint16_t multicastPort=22;

  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                      Address (InetSocketAddress (multicastGroup, multicastPort)));
  onoff.SetConstantRate (DataRate ("300b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));
  //establecer una que utilice udp para video
  ApplicationContainer app_s=onoff.Install(c_serv.Get(0));
  app_s.Start (Seconds (1.));
  app_s.Stop (Seconds (10.));
  
  //establecer que utilice por ejemplo tcp para texto

  //instalamos en los receptores aplicaciones para recibir los paquetes
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), multicastPort));
  ApplicationContainer app_c = sink.Install (c_univ);

  app_c.Start (Seconds (1.));
  app_c.Stop (Seconds (10.));

  Observador* cliente_observador=new Observador(app_c.Get(1)->GetObject<PacketSink>());
  
  NS_LOG_INFO("Arranca simulación");
    
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();
  NS_LOG_INFO("Numero total de paquetes " << cliente_observador->TotalPaquetes());
  NS_LOG_INFO("Fin simulación");

}

Ptr<Node> PuenteHelper (NodeContainer nodosLan,NetDeviceContainer & d_nodosLan,DataRateValue c_transmision)
{
  NetDeviceContainer d_puertosBridge;
  CsmaHelper h_csma;
  BridgeHelper h_bridge;
  Ptr<Node> puente = CreateObject<Node> ();
  h_csma.EnablePcapAll("envio");
  h_csma.SetChannelAttribute ("DataRate", c_transmision);
  for (NodeContainer::Iterator indice = nodosLan.Begin ();
       indice != nodosLan.End ();
       indice++)
    {
      NetDeviceContainer enlace =
	h_csma.Install (NodeContainer (*indice, puente));
      d_nodosLan.Add (enlace.Get (0));
      d_puertosBridge.Add (enlace.Get (1));
    }
  h_bridge.Install (puente, d_puertosBridge);
  return puente;
}
