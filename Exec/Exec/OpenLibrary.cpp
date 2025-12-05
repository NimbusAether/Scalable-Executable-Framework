#include <Windows.h>
#include "OpenLibrary.h"
#include "log.h"

using namespace SEF::Exec;
using SEF::Exec::DllInfo;
using SEF::Exec::OpenLibrary;
typedef ElementFactory* (*CreateFactory)();

class DllInfo 
{
public:
	DllInfo() 
	{
	};
	~DllInfo()
	{
	}
	HINSTANCE hDll = nullptr;
	CreateFactory m_createFunc;
};

OpenLibrary::OpenLibrary()
{
	m_info = new DllInfo();
}

OpenLibrary::~OpenLibrary()
{
	delete m_info;
}

ElementFactory* OpenLibrary::LoaderFactory(const char* dllName)
{
	m_info->hDll = LoadLibrary(dllName);
	if (m_info->hDll == nullptr) 
	{
		LOGFMTE("open %s failed:%s---", dllName, strerror(errno));
		return nullptr;
	}
	LOGFMTT("open %s success\n", dllName);

	m_info->m_createFunc = (CreateFactory)GetProcAddress(m_info->hDll, "CreateFactory");
	if (m_info->m_createFunc == nullptr) 
	{
		LOGFMTE("load func CreateFactory failed:%s\n", dllName);
		return nullptr;
	}
	ElementFactory* factory = m_info->m_createFunc();
	return factory;
}
