#include "ns3/ptr.h"
#include "ns3/udp-server.h"
#include "ns3/packet-sink.h"
#include "ns3/ipv4-address.h"
#include "ns3/onoff-application.h"
#include "ns3/csma-net-device.h"


using namespace ns3;

class Observador
{
public:
           Observador      (Ptr<PacketSink>    servidor,Ptr<CsmaNetDevice> rx,Ptr<CsmaNetDevice> tx);
           Observador      (Ptr<OnOffApplication>    app);
  void     PaqueteRecibido (Ptr<const Packet> paquete,const Address &address);
  void     PaqueteEnviado (Ptr<const Packet> paquete);
  void Paq_rx(Ptr<const Packet> paquete);
  void Paq_tx(Ptr<const Packet> paquete);
  
  uint32_t TotalPaquetes   ();
  uint32_t TotalPaquetes_enviado  ();
  uint32_t TotalPaquetes_perdidos  ();
  uint32_t TotalPaquetes_recibidos  ();
private:
  uint32_t m_cuenta;
  uint32_t m_paquetes_enviados;
  uint32_t paq_rx;
  uint32_t paq_tx;
};