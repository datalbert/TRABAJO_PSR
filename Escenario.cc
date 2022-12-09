//ESCENARIO PROYECTO DE LA ASIGNATURA PLANIFICACIÓN Y SIMULACIÓN DE REDES


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

using namespace ns3;

//Componente de trazado como proyecto
NS_LOG_COMPONENT_DEFINE("Proyecto");

Ptr<Node> PuenteHelper (NodeContainer nodosLan, NetDeviceContainer & d_nodosLan);

int main (int arcg, char *argv[])
{

  Time::SetResolution(Time::NS);

  Time duracion = Time("60s");
  Time intervalo = Time("1s");	
  std::string regBin="100kbps";
  int tamPaq_video = 1024;
  int tamPaq_texto = 820;
  int tam_cola=10000;

  //Línea de comandos
  CommandLine cmd;
  cmd.AddValue("duracion","duración de la simulación",duracion);
  cmd.AddValue("intervalo","tiempo entre dos paquetes consecutivos",intervalo);
  cmd.AddValue("regBin", "régimen binario", regBin);	
  cmd.AddValue("tamPaq_video","tamaño medio de los paquetes de videos en octetos ",tamPaq_video);
  cmd.AddValue("tamPaq_texto","tamaño medio de los paquetes de texto en octetos ",tamPaq_texto);
  cmd.AddValue("tam_cola","tamaños de la cola del servidor en octetos",tam_cola);
  cmd.Parse (arcg,argv);

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
  c_serv.Create(2);
  

  Names::Add ("servidor1",c_serv.Get(0));//hacemos un nombrado a los nodos para posteriormente poder buscarlo mas sencillo
  Names::Add ("servidor2",c_serv.Get(1));

  //Creamos un contenedor donde meter todos los nodos
  NodeContainer c_todos;
  c_todos.Add (c_serv);
  c_todos.Add(c_univ);

  
  //Añadimos la pila
  InternetStackHelper h_pila; 
  h_pila.SetIpv6StackInstall (false);
  //h_pila.SetIpv4StackInstall (true);  
  h_pila.Install (c_todos);
  
  //Utilizamos PointToPointHelper para crear las interfaces y los enlaces punto a punto.
  
  NetDeviceContainer dispositivos;

  //Utilizamos PuenteHelper para conectar los servidores y los clientes.
  Ptr<Node> puente = PuenteHelper(c_todos, dispositivos);

 

   NS_LOG_INFO("Arranca simulación");
    
    Simulator::Stop(duracion);
    Simulator::Run();
  NS_LOG_INFO("Direccion IP del servidor:" << puente->GetDevice(0)->GetAddress());

   NS_LOG_INFO("Fin simulación");

}


Ptr<Node> PuenteHelper (NodeContainer nodosLan,NetDeviceContainer & d_nodosLan)
{
  NetDeviceContainer d_puertosBridge;
  CsmaHelper h_csma;
  BridgeHelper h_bridge;
  Ptr<Node> puente = CreateObject<Node> ();
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
