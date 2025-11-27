#ifndef _LWIP_BASE_H
#define _LWIP_BASE_H

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
#define LWIP_C_LINKAGE_BEGIN extern "C"{
#define LWIP_C_LINKAGE_END }
#else
#define LWIP_C_LINKAGE_BEGIN 
#define LWIP_C_LINKAGE_END 
#endif

typedef void* LwipUdpHandle;
#endif