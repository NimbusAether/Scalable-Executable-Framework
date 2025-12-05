#ifndef _LWIP_UDP_H
#define _LWIP_UDP_H
#include "lwipBase.h"
LWIP_C_LINKAGE_BEGIN

typedef void(*onRecv)(void *arg, LwipUdpHandle upcb,
                 const void *recvBuf, int size,const char *remoteIp, unsigned short remmotePort);
EXPORT_API LwipUdpHandle lwipUdpCreate();

EXPORT_API void lwipUdpRemove(LwipUdpHandle pcb);
EXPORT_API char lwipUdpBind(LwipUdpHandle pcb, const char *ip,unsigned short port);
EXPORT_API char lwipUdpConnect(LwipUdpHandle pcb, const char *dst_ip,unsigned short dst_port);
EXPORT_API void lwipUdpDisConnect(LwipUdpHandle pcb);
EXPORT_API void lwipUdpRecv(LwipUdpHandle pcb,onRecv cb,void *recv_arg);
EXPORT_API char lwipUdpSendto(LwipUdpHandle pcb, const void *recvBuf, int size, const char *dst_ip, unsigned short dst_port);
EXPORT_API char lwipUdpSend(LwipUdpHandle pcb, const void *recvBuf, int size);
LWIP_C_LINKAGE_END

#endif

