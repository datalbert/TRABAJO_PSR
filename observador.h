#include "ns3/ptr.h"
#include "ns3/udp-server.h"
#include "ns3/packet-sink.h"

using namespace ns3;

class Observador
{
public:
           Observador      (Ptr<PacketSink>    servidor);
  void     PaqueteRecibido (Ptr<const Packet> paquete,const Address &address);
  uint32_t TotalPaquetes   ();

private:
  uint32_t m_cuenta;
};