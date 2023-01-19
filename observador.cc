#include "ns3/object-base.h"
#include "ns3/log.h"
#include "observador.h"
#include "ns3/ipv4-address.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Observador");


Observador::Observador (Ptr<PacketSink> servidor,Ptr<CsmaNetDevice> rx,Ptr<CsmaNetDevice> tx)
{
  m_cuenta = 0;
  paq_rx=0;
  paq_tx=0;
  servidor->TraceConnectWithoutContext ("Rx",
                                        MakeCallback(&Observador::PaqueteRecibido,
                                                     this)); 
  rx->TraceConnectWithoutContext ("MacRx",
                                        MakeCallback(&Observador::Paq_rx,
                                                     this));  
  tx->TraceConnectWithoutContext ("MacTx",
                                        MakeCallback(&Observador::Paq_tx,
                                                     this));   

}

Observador::Observador(Ptr<OnOffApplication> app){
  m_paquetes_enviados=0;
  app->TraceConnectWithoutContext ("Tx",
                                        MakeCallback(&Observador::PaqueteEnviado,
                                                     this)); 
}
void
Observador::PaqueteRecibido (Ptr<const Packet> paquete,const Address &address)
{
  
  NS_LOG_FUNCTION ("Se ha recibido el paquete " << paquete);
  m_cuenta++;
}
void
Observador::PaqueteEnviado (Ptr<const Packet> paquete)
{
  
  NS_LOG_FUNCTION ("Se ha enviado el paquete " << paquete);
  m_paquetes_enviados++;
}

void
Observador::Paq_rx (Ptr<const Packet> paquete)
{
  
  paq_rx++;
}
void
Observador::Paq_tx (Ptr<const Packet> paquete)
{
  
  paq_tx++;
}

uint32_t
Observador::TotalPaquetes ()
{
  return m_cuenta;
}

uint32_t
Observador::TotalPaquetes_enviado ()
{
  return m_paquetes_enviados;
}

uint32_t
Observador::TotalPaquetes_recibidos ()
{
  return paq_rx;
}

uint32_t
Observador::TotalPaquetes_perdidos ()
{
  return (paq_rx-paq_tx);
}