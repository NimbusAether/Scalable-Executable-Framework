#pragma once

#include "Udm.h"
#include "UdmRequestQueue.h"
#include "ByteArray.h"

extern "C" UDM_EXPORT_API SEF::Udm::Udm* CreateUdmByType(int type);

extern "C" UDM_EXPORT_API SEF::Request * CreateRequest();

extern "C" UDM_EXPORT_API SEF::Parameter* CreateParamter();

extern "C" UDM_EXPORT_API SEF::COMMON::ByteArray * CreateByteArray();


class SEF_SDK :public SEF::Parameter {
public:
	SEF_SDK();
	~SEF_SDK();

};