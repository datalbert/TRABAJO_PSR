
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
#include "ns3/csma-net-device.h"
#include "ns3/bridge-helper.h"
#include "ns3/queue.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/queue-disc-container.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/on-off-helper.h"
#include "observador.h"
#include "ns3/socket.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/gnuplot.h"
#include "ns3/csma-net-device.h"
#include <iostream>
#include <string>
#include "ns3/traffic-control-layer.h"



using namespace ns3;

//Componente de trazado como proyecto
NS_LOG_COMPONENT_DEFINE("Proyecto");

Ptr<Node> PuenteHelper (NodeContainer nodosLan, NetDeviceContainer & d_nodosLan,DataRateValue c_transmision);
double escenario(int decision,int num_clientes_viapol,int num_clientes_cartuja,int num_clientes_reina_mercedes,int num_clientes_rectorado,Time duracion_simulacion,Time intervalo_video,Time intervalo_texto,DataRateValue c_transmision,uint16_t port_servidor);

int main (int arcg, char *argv[])
{

  Time::SetResolution(Time::NS);

  Time duracion = Time("20s");
  Time intervalo = Time("1s");	
  std::string intervalo_video_value="100us";
  std::string intervalo_texto_value="50us";
  std::string regBin="300kbps";
  int num_clientes_viapol=5;
  int num_clientes_cartuja=0;
  int num_clientes_reina_mercedes=12;
  int num_clientes_rectorado=80;
  int tamPaq_video = 1024;
  int tamPaq_texto = 820;
  int tam_cola=10000;
  int num_puntos=11;
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
  cmd.AddValue("clientes_rectorado","numero de clientes a conectados en la red del rectorado",num_clientes_rectorado);
  cmd.AddValue("clientes_viapol","numero de clientes a conectados en la red del rectorado",num_clientes_viapol);
  cmd.AddValue("clientes_cartuja","numero de clientes a conectados en la red del rectorado",num_clientes_cartuja);
  cmd.AddValue("clientes_reina_mercedes","numero de clientes a conectados en la red del rectorado",num_clientes_reina_mercedes);
  cmd.Parse (arcg,argv);


  Time intervalo_video =Time(intervalo_video_value);
  Time intervalo_texto =Time(intervalo_texto_value);

  DataRateValue RegBin = DataRateValue(regBin);
  int i=0;
  double retardo=escenario(i,num_clientes_viapol,num_clientes_cartuja,num_clientes_reina_mercedes,num_clientes_rectorado,duracion,intervalo_video,intervalo_texto,RegBin,port_servidor);

  /*
  //creacion de la gráfica
  std::string fileNameWithNoExtension = "grafica_trabajo";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = "% paq perdidos por ancho de banda en kb/s";
  std::string dataTitle               = "Campus";

  // Instantiate the plot and set its title.
  Gnuplot plot(graphicsFileName);
  plot.SetTitle(plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal("png");

  // Set the labels for each axis.
  plot.SetLegend("Reg bin kb/s", "% Dropped Packets");


  // Instantiate the dataset, set its title, and make the points be
  // plotted along with connecting lines.
  for (int i=0;i<=5;i++){
    Gnuplot2dDataset dataset;
    
    if (i==0)
      {
       
       dataTitle="Campus rectorado con " + std::to_string(num_clientes_rectorado);
       NS_LOG_INFO("RECTORADO");
      }
    else if(i==1)
      {
        
        dataTitle="Campus reina mercedes con " +  std::to_string(num_clientes_reina_mercedes);
        NS_LOG_INFO("REINA MERCEDES");
      }
    else if(i==2)
      {
        
        dataTitle="Campus viapol con " + std::to_string(num_clientes_viapol);
        NS_LOG_INFO("VIAPOL");
      }
    else if(i==3)
      {
        NS_LOG_INFO("CARTUJA");
        dataTitle="Campus cartuja con " + std::to_string(num_clientes_cartuja);
      }
    else if(i==4)
      {
        dataTitle="Umbral máximo permitido";
      }
    else 
      {
        dataTitle="Umbral mínimo permitido";
      }
      
    dataset.SetTitle(dataTitle);
    dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    uint64_t regbin_inicial=650000;
    double regimen=650;
    NS_LOG_INFO("-----------------------------------------------");
    if (i<4){
      for (int j=0;j<num_puntos;j++){
        DataRateValue regbin_simu=DataRateValue(regbin_inicial);
        NS_LOG_INFO("reg_bin " << regbin_inicial);
        double retardo=escenario(i,num_clientes_viapol,num_clientes_cartuja,num_clientes_reina_mercedes,num_clientes_rectorado,duracion,intervalo_video,intervalo_texto,regbin_simu,port_servidor);
        NS_LOG_INFO("EL porcentaje de paqutes perdidos es "<< retardo);
        dataset.Add(regimen,retardo);
        regimen-=10;
        regbin_inicial-=10000;
      }
    }
    if (i==4){
      for (int j=0;j<num_puntos;j++){
        
        double umbral_max=20;
        dataset.Add(regimen,umbral_max);
        regimen-=10;
      }
    }
    else if(i==5){
      for (int j=0;j<num_puntos;j++){
        
        double umbral_min=10;
        dataset.Add(regimen,umbral_min);
        regimen-=10;
      }
    }
    plot.AddDataset(dataset);
    NS_LOG_INFO("---------------------------------------------");
  }
  std::ofstream plotFile(plotFileName.c_str());

  // Write the plot file.
  plot.GenerateOutput(plotFile);

  // Close the plot file.
  plotFile.close();
  */


}

double escenario(int decision,int num_clientes_viapol,int num_clientes_cartuja,int num_clientes_reina_mercedes,int num_clientes_rectorado,Time duracion_simulacion,Time intervalo_video, Time intervalo_texto,DataRateValue c_transmision,uint16_t port_servidor){
  
  //Definimos el contenedor de nodos de universidades
  Simulator::Destroy();
  NS_LOG_DEBUG("Creating nodecontainers for university");
  NodeContainer c_rectorado;
  NodeContainer c_viapol;
  NodeContainer c_reina_mercedes;
  NodeContainer c_cartuja;
  
  //Creamos tantos los clientes en cada campus y le sumamos 1 porque va a ser el nodo como de router 
  NS_LOG_DEBUG("Creating the clients in nodecontainer of universitys");
  c_rectorado.Create(num_clientes_rectorado+1);
  c_viapol.Create(num_clientes_viapol+1);
  c_cartuja.Create(num_clientes_cartuja+1);
  c_reina_mercedes.Create(num_clientes_reina_mercedes+1);

 

  //Creamos nodos de servidores
  NS_LOG_DEBUG("Creating the serv container");
  NodeContainer c_serv;
  c_serv.Create(1);

  NS_LOG_DEBUG("Creating a nodecontainer how englove all nodos in the network");
  NodeContainer c_todos;
  c_todos.Add(c_serv);
  c_todos.Add(c_viapol);
  c_todos.Add(c_reina_mercedes);
  c_todos.Add(c_rectorado);
  c_todos.Add(c_cartuja);
  
  NetDeviceContainer c_dispositivos;

  //Añadimos la pila

  InternetStackHelper h_pila; 
  h_pila.SetIpv6StackInstall (false); 
  h_pila.Install(c_todos);

  //Creamos un contenedor donde meter todos los nodos que se van a conectar al conmutador
  NodeContainer c_nucleo;
  c_nucleo.Add (c_serv.Get(0));
  c_nucleo.Add(c_rectorado.Get(0));
  c_nucleo.Add(c_reina_mercedes.Get(0));
  c_nucleo.Add(c_viapol.Get(0));
  c_nucleo.Add(c_cartuja.Get(0));
  //instalamos un canal csma en cada campus
  
  NS_LOG_DEBUG("Instaling a csma channel for each campus");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate ("900kb/s")));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (1)));

  NetDeviceContainer nd_viapol = csma.Install (c_viapol);  
  NetDeviceContainer nd_cartuja = csma.Install (c_cartuja);  
  NetDeviceContainer nd_reina_mercedes = csma.Install (c_reina_mercedes);  
  NetDeviceContainer nd_rectorado = csma.Install (c_rectorado);


  //Utilizamos PuenteHelper para conectar los servidores y los clientes.
  Ptr<Node> puente = PuenteHelper(c_nucleo, c_dispositivos,c_transmision);

  //Asignamos direcciones IPv4

  NS_LOG_DEBUG("Assing ip addresse to each LAN");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer  ip_nucleo =  ipv4Addr.Assign (c_dispositivos);
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer  ip_viapol =  ipv4Addr.Assign (nd_viapol);
  ipv4Addr.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer  ip_cartuja =  ipv4Addr.Assign (nd_cartuja);
  ipv4Addr.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer  ip_reina_mercedes =  ipv4Addr.Assign (nd_reina_mercedes);
  ipv4Addr.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer  ip_rectorado =  ipv4Addr.Assign (nd_rectorado);
 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //creamos dos aplicaciones de OnOff en el servidor una para trafico de video y otra para texto
  NS_LOG_DEBUG("Creating the OnOffApplication in the differents sources");

  uint16_t port=22;

  
  OnOffHelper onoff_udp ("ns3::UdpSocketFactory", 
                      Address (InetSocketAddress (ip_nucleo.GetAddress(0), port)));
  onoff_udp.SetConstantRate (DataRate ("400kb/s"));
  onoff_udp.SetAttribute ("PacketSize", UintegerValue (300));
  NS_LOG_DEBUG("Instalacion de las fuentes en los equipos de los campus");
  //installar aplicaciones en cad uno de los campus
  ApplicationContainer app_sources_viapol;
  NS_LOG_DEBUG(c_viapol.GetN());
  for (int i=1;i<c_viapol.GetN();i++){
    app_sources_viapol.Add(onoff_udp.Install(c_viapol.Get(i)));
  }
  ApplicationContainer app_sources_rectorado;
  for (int i=1;i<c_rectorado.GetN();i++){
    app_sources_rectorado.Add(onoff_udp.Install(c_rectorado.Get(i)));
  }
  ApplicationContainer app_sources_cartuja;
  for (int i=1;i<c_cartuja.GetN();i++){
    app_sources_cartuja.Add(onoff_udp.Install(c_cartuja.Get(i)));
  }
  NS_LOG_DEBUG("aqui");
  ApplicationContainer app_sources_reina_mercedes;
  for (int i=1;i<c_reina_mercedes.GetN();i++){
    app_sources_reina_mercedes.Add(onoff_udp.Install(c_reina_mercedes.Get(i)));
  }
  
  
 //instalamos en los receptores aplicaciones para recibir los paquetes
  PacketSinkHelper sink_udp ("ns3::UdpSocketFactory",
                            InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer app_l = sink_udp.Install (c_nucleo.Get(0));
  app_l.Add(sink_udp.Install (c_rectorado.Get(0)));
  app_l.Start (Seconds (0.1));
  app_l.Stop (duracion_simulacion);
  //NS_LOG_DEBUG("SE HA CREADO EL ESCENARIO");//Ptr<Queue<Packet>> cola_puente_rectorado = c_dispositivos.Get(3)->GetObject<CsmaNetDevice>()->GetQueue();
  
  //cola de control de trafico en el rectorado

  Ptr<TrafficControlLayer> tcl = c_nucleo.Get(1)->GetObject<TrafficControlLayer> ();
  Ptr<QueueDisc> cola_rectorado = tcl->GetRootQueueDiscOnDevice(puente->GetDevice(3));

  Ptr<Queue<Packet>> cola_puente_rectorado = puente->GetDevice(3)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_puente_cartuja = c_dispositivos.Get(4)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_puente_viapol = c_dispositivos.Get(1)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_puente_reina_mercedes = c_dispositivos.Get(2)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_puente_serv = c_dispositivos.Get(0)->GetObject<CsmaNetDevice>()->GetQueue();
  
  //puntero para obtener los paquetes recibidos== tx en el 
  
  Ptr<Queue<Packet>> cola_router_rectorado = nd_rectorado.Get(0)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_router_cartuja = nd_cartuja.Get(0)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_router_viapol = nd_viapol.Get(0)->GetObject<CsmaNetDevice>()->GetQueue();
  Ptr<Queue<Packet>> cola_router_reina_mercedes = nd_reina_mercedes.Get(0)->GetObject<CsmaNetDevice>()->GetQueue();

  Ptr<CsmaNetDevice> router_rectorado=nd_rectorado.Get(0)->GetObject<CsmaNetDevice>();
  Ptr<CsmaNetDevice> router_cartuja=nd_cartuja.Get(0)->GetObject<CsmaNetDevice>();
  Ptr<CsmaNetDevice> router_viapol=nd_viapol.Get(0)->GetObject<CsmaNetDevice>();
  Ptr<CsmaNetDevice> router_reina_mercedes=nd_reina_mercedes.Get(0)->GetObject<CsmaNetDevice>();

  Ptr<CsmaNetDevice> puente_rectorado=c_dispositivos.Get(3)->GetObject<CsmaNetDevice>();
  Ptr<CsmaNetDevice> puente_cartuja=c_dispositivos.Get(4)->GetObject<CsmaNetDevice>();
  Ptr<CsmaNetDevice> puente_viapol=c_dispositivos.Get(1)->GetObject<CsmaNetDevice>();
  Ptr<CsmaNetDevice> puente_reina_mercedes=c_dispositivos.Get(2)->GetObject<CsmaNetDevice>();


  Observador* observador_rectorado=new Observador(app_l.Get(0)->GetObject<PacketSink>(),router_rectorado,puente_rectorado);
  Observador* observador_cartuja=new Observador(app_l.Get(0)->GetObject<PacketSink>(),router_cartuja,puente_cartuja);
  Observador* observador_reina_mercedes=new Observador(app_l.Get(0)->GetObject<PacketSink>(),router_reina_mercedes,puente_reina_mercedes);
  Observador* observador_viapol=new Observador(app_l.Get(0)->GetObject<PacketSink>(),router_viapol,puente_viapol);
  Observador* cliente_observador_router_rectorado=new Observador(app_sources_viapol.Get(0)->GetObject<OnOffApplication>());
 
  

  //vemos el tamaño de cola del dispositivo para los 4 equipos

  

  
  NS_LOG_INFO("Arranca simulación");
  
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();
  
  //NS_LOG_INFO("Numero total de paquetes " << observador_rectorado->TotalPaquetes() << " recibidos en el  servidor");
  //NS_LOG_INFO("Numero total de paquetes " << cliente_observador_router_rectorado->TotalPaquetes_enviado() << " enviados por una fuente de viapol");
  /*
  NS_LOG_DEBUG("TOTAL DE PAQUETES RECIBIDOR POR EL ROUTER DE VIAPOL " << cola_router_viapol->GetTotalReceivedPackets());
  NS_LOG_DEBUG("TOTAL DE PAQUETES RECIBIDOR POR EL ROUTER DE RECTORADO " << cola_router_rectorado->GetTotalReceivedPackets());
  NS_LOG_DEBUG("TOTAL DE PAQUETES PERDIDOS POR EL ROUTER DE RECTORADO " << cola_router_rectorado->GetTotalDroppedPackets());
  */
  double paq_perdidos_rectorado=100*((double)observador_rectorado->TotalPaquetes_perdidos()/(double)observador_rectorado->TotalPaquetes_recibidos());
  double paq_perdidos_reina_mercedes=100*((double)observador_reina_mercedes->TotalPaquetes_perdidos()/(double)observador_reina_mercedes->TotalPaquetes_recibidos());
  double paq_perdidos_viapol=100*((double)observador_viapol->TotalPaquetes_perdidos()/(double)observador_viapol->TotalPaquetes_recibidos());
  double paq_perdidos_cartuja=100*((double)observador_cartuja->TotalPaquetes_perdidos()/(double)observador_cartuja->TotalPaquetes_recibidos());
  
  

  //usar este
  //double porcentaje_paquetes_perdidos = 100*(double)cola_servidor->GetTotalDroppedPackets() / (double)(retardo.TotalPaquetesTx());

  double valor_devuelto;
  
  NS_LOG_INFO("paquetes perdidos por el rectorado " << cola_puente_rectorado->GetTotalDroppedPackets());
  NS_LOG_INFO("paquetes perdidos por el rectorado " << cola_router_rectorado->GetTotalDroppedPackets());
  NS_LOG_INFO("paquetes perdidos por el rectorado " << cola_rectorado->GetTotalDroppedPackets());
  NS_LOG_INFO("PAQUETES RECIBIDOS POR EL RECTORADO por el router "<<cola_router_rectorado->GetTotalReceivedPackets());
  NS_LOG_INFO("PAQUETES RECIBIDOS POR EL RECTORADO  por el puerto"<<cola_puente_rectorado->GetTotalReceivedPackets());
  //NS_LOG_INFO("PAQ PERDIDOS RETORADO " << cliente_observador_servidor->TotalPaquetes_perdidos());
  if(decision==0)
    {
      valor_devuelto=paq_perdidos_rectorado;
    }
  else if(decision==1)
    {
      valor_devuelto=paq_perdidos_reina_mercedes;
    }
      
  else if(decision==2)
    {
      valor_devuelto=paq_perdidos_viapol;
    }
  else
    {
      valor_devuelto=paq_perdidos_cartuja;
    }
 
  NS_LOG_INFO("Fin simulación");

  return valor_devuelto;

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
      NetDeviceContainer enlace =h_csma.Install (NodeContainer (*indice, puente));
      d_nodosLan.Add (enlace.Get (0));
      d_puertosBridge.Add (enlace.Get (1));
    }
  h_bridge.Install (puente, d_puertosBridge);
  return puente;
}
