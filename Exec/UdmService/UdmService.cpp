#include "UdmService.h"

using namespace SEF::UdmService;

UdmService::UdmService()
{

}

UdmService::~UdmService()
{

}

RunningStat UdmService::GetStat()
{
	return m_stat;
}
unsigned int UdmService::getFrequency()
{
	return m_frequency;
}

const string UdmService::getAddressUrl()
{
	return m_addressUrl;
}