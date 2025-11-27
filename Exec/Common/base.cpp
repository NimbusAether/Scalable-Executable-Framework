#include "base.h"
#include "platform.h"
using namespace SEF::COMMON;

#ifdef VLD_DEBUG
#include "vld.h"
#endif

SEF_Uint32 SwapEndian::swap2local(SEF_Uchar *data, SEF_Uint32 &result) 
{
#if defined(BIG_ENDIAN)
	result = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
#else
	result = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
#endif
	return result;
}

SEF_Uint32 SwapEndian::swap2local(SEF_Uint32 data, SEF_Uint32& result)
{
	SEF_Uchar* bytes = (SEF_Uchar*)&data;

#if defined(BIG_ENDIAN)
	result = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
#else
	result = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
#endif
	return result;
}

SEF_Uint16 SwapEndian::swap2local(SEF_Uchar *data, SEF_Uint16 &result) 
{
#if defined(BIG_ENDIAN)
	result = (data[0] << 8) | data[1];
#else
	result = (data[1] << 8) | data[0];
#endif
	return result;
}

SEF_Uint16 SwapEndian::swap2local(SEF_Uint16 data, SEF_Uint16& result)
{
	SEF_Uchar* bytes = (SEF_Uchar*)&data;
#if defined(BIG_ENDIAN)
	result = (bytes[0] << 8) | bytes[1];
#else
	result = (bytes[1] << 8) | bytes[0];
#endif
	return result;
}

void SwapEndian::swap2network(SEF_Uchar *data,  SEF_Uint32 tmp) 
{
#if defined(BIG_ENDIAN)
	data[0] = (tmp >> 24) & 0xff;
	data[1] = (tmp >> 16) & 0xff;
	data[2] = (tmp >> 8) & 0xff;
	data[3] = tmp & 0xff;
#else
	data[3] = (tmp >> 24) & 0xff;
	data[2] = (tmp >> 16) & 0xff;
	data[1] = (tmp >> 8) & 0xff;
	data[0] = tmp & 0xff;
#endif
}

void SwapEndian::swap2network(SEF_Uchar *data,  SEF_Uint16  tmp) 
{
#if defined(BIG_ENDIAN)
	data[0] = (tmp >> 8) & 0xff;
	data[1] = tmp & 0xff;
#else
	data[1] = (tmp >> 8) & 0xff;
	data[0] = tmp & 0xff;
#endif
}
