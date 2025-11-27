#include "CentralizationKernel.h"
#include <stdio.h>
#include <chrono>
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using SEF::Kernel::CentralizationKernel;
using SEF::Element::Element;
using SEF::Kernel::ModeType;


CentralizationKernel::CentralizationKernel() 
{
	m_currentCount = 0;
}

CentralizationKernel::~CentralizationKernel() 
{
	m_isRunning = false;
	m_stackTimer.stop();
	m_stackTimer.cancelTimer(this);

	for (auto it = m_threadWrappers.begin();it!= m_threadWrappers.end();)
	{
		ElementThreadWrapper* wrapper = it->second;
		it = m_threadWrappers.erase(it);
		delete wrapper;
	}
	m_threadWrappers.clear();
}

bool CentralizationKernel::AddElementsGroup(Group* group)
{
	elements_group.push_back(group);
	vector<Element::Element*> elems = group->GetElements();
	m_allElements.insert(m_allElements.end(), elems.begin(), elems.end());
	return false;
}

bool CentralizationKernel::AddElement(Element::Element* element)
{
	elements.push_back(element);
	m_allElements.push_back(element);
	return false;
}

vector<Group*> CentralizationKernel::GetElementsGroup() 
{
	return elements_group;
}

vector<Element*> CentralizationKernel::GetElementsNameByGroupName(string elementsGroupName) 
{
	return elements;
}

void SEF::Kernel::CentralizationKernel::timeoutCB()
{
	/*
	* 记录上一次kernel 运行周期，element运行实际频率
	*/
	if (m_isRunning == false)
	{
		return;
	}
	m_currentCount++;
	LOGFMTT("==========Kernel timer Start======[%d]=====", m_currentCount);
	std::vector<elementStat> vec;
	float ratio = 0.00;
	for (const auto& pair : m_threadWrappers) 
	{
		ElementThreadWrapper* wrapper = pair.second;
		Element::Element * element = wrapper->getElement();
		wrapper->Pause(ratio);
		LOGFMTD("m_currentCount %d: element %s at mode %s: ratio %f", m_currentCount,
			element->GetElementName().c_str(), SEF::Kernel::modeType2String(element->GetElementMode()), ratio);
		elementStat stat;
		stat.elementName = element->GetElementName();
		stat.failedCount = (1.00 - ratio) * /*element->GetElementFrequency()*/frequency;
		stat.successedCount = ratio * /*element->GetElementFrequency()*/frequency;
		stat.status = (SEF_Int)element->GetElementMode();
		vec.push_back(stat);
	}
	

	for (const auto& pair : m_threadWrappers) 
	{
		ElementThreadWrapper* wrapper = pair.second;
		wrapper->Reset();
	}
	/*
	* 开始下一次kernel运行周期
	*/
	m_stackTimer.addTimer(this, 1000);
	LOGFMTT("==========Kernel timer End===========");
	m_statusPublisher->countPublisher(vec);
}

void CentralizationKernel::ExecuteKernel()
{
	if (m_isRunning == false) 
	{
		initElements();
		m_isRunning = true;

		Startup("");
		m_stackTimer.addTimer(this, 1000);
	}
	LOGFMTT("Centralization Kernel execute\n");
}

bool CentralizationKernel::ElementStatControl(const string& elementName, ModeType modeType)
{
	bool ret = false;
	switch (modeType)
	{
		case ModeType::Freeze:{
			ret = Freeze(elementName);
			break;
		}
		case ModeType::Startup: {
			ret = Startup(elementName);
			break;
		}
		case ModeType::Initialize: {
			ret = Initialize(elementName);
			break;
		}
		case ModeType::Run: {
			ret = Run(elementName);
			break;
		}
		case ModeType::Shutdown: {
			ret = Shutdown(elementName);
			break;
		}
		default: {
			LOGFMTE("unsupport modeType(%d) change for %s", modeType, elementName.c_str());
			break;
		}
	}
	return ret;
}

void SEF::Kernel::CentralizationKernel::setElemeltPublisherCb(elementStatusPublisher* cb)
{
	m_statusPublisher = cb;
}

bool CentralizationKernel::initElements()
{
	for (Element::Element* elem : m_allElements) 
	{
		string tName= elem->GetElementName();
		ElementThreadWrapper* wrapper = new ElementThreadWrapper(elem, m_stackTimer, frequency);
		wrapper->StartThread();
		auto result = m_threadWrappers.insert(std::make_pair(tName, wrapper));
		if (result.second == false) 
		{
			LOGFMTT("%s element run failed, drop it, Please make sure that element name is unique\n", elem->GetElementName().c_str());
		}
		//m_stackTimer.addTimer(wrapper, 1000);
	}
	return true;
}

Element* CentralizationKernel::FindElement(const string& elementName)
{
	Element::Element* element = nullptr;
	auto it = m_threadWrappers.find(elementName);
	if (it != m_threadWrappers.end()) 
	{
		ElementThreadWrapper *wrapper = it->second;
		element = wrapper->getElement();
	}
	return element;
}

bool CentralizationKernel::PublishOutputs(const string& elementName)
{

	return false;
}

bool CentralizationKernel::SubscribeInputs(const string& elementName)
{
	return false;
}

bool CentralizationKernel::Startup(const string& elementName)
{
	if (elementName.empty()) 
	{
		for (const auto & pair: m_threadWrappers) 
		{
			ElementThreadWrapper* wrapper = pair.second;
			wrapper->Init();
		}
	}
	else 
	{
		auto it = m_threadWrappers.find(elementName);
		if (it != m_threadWrappers.end()) 
		{
			ElementThreadWrapper* wrapper = it->second;
			wrapper->Init();
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool CentralizationKernel::Initialize(const string& elementName)
{
	if (elementName.empty())
	{
		for (const auto& pair : m_threadWrappers)
		{
			ElementThreadWrapper* wrapper = pair.second;
			wrapper->Init();
		}
	}
	else
	{
		auto it = m_threadWrappers.find(elementName);
		if (it != m_threadWrappers.end())
		{
			ElementThreadWrapper* wrapper = it->second;
			wrapper->Init();
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool SEF::Kernel::CentralizationKernel::Freeze(const string& elementName)
{
	if (elementName.empty())
	{
		for (const auto& pair : m_threadWrappers)
		{
			ElementThreadWrapper* wrapper = pair.second;
			wrapper->SetModeType(ModeType::Freeze);
		}
	}
	else
	{
		auto it = m_threadWrappers.find(elementName);
		if (it != m_threadWrappers.end())
		{
			ElementThreadWrapper* wrapper = it->second;
			wrapper->SetModeType(ModeType::Freeze);
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool SEF::Kernel::CentralizationKernel::Run(const string& elementName)
{
	if (elementName.empty())
	{
		for (const auto& pair : m_threadWrappers)
		{
			ElementThreadWrapper* wrapper = pair.second;
			wrapper->SetModeType(ModeType::Run);
		}
	}
	else
	{
		auto it = m_threadWrappers.find(elementName);
		if (it != m_threadWrappers.end())
		{
			ElementThreadWrapper* wrapper = it->second;
			wrapper->SetModeType(ModeType::Run);
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool SEF::Kernel::CentralizationKernel::Shutdown(const string& elementName)
{
	if (elementName.empty())
	{
		for (const auto& pair : m_threadWrappers)
		{
			ElementThreadWrapper* wrapper = pair.second;
			wrapper->SetModeType(ModeType::Shutdown);
		}
	}
	else
	{
		auto it = m_threadWrappers.find(elementName);
		if (it != m_threadWrappers.end())
		{
			ElementThreadWrapper* wrapper = it->second;
			wrapper->SetModeType(ModeType::Shutdown);
		}
		else
		{
			return false;
		}
	}
	return true;
}

ElementThreadWrapper* CentralizationKernel::FindWrapper(const string& elementName)
{
	ElementThreadWrapper* wrapper = NULL;
	auto it = m_threadWrappers.find(elementName);
	if (it != m_threadWrappers.end()) 
	{
		wrapper = it->second;
	}
	return wrapper;
}


/***************element work thread******************/
using SEF::Kernel::ElementThreadWrapper;
using SEF::Kernel::ElementWorker;

inline static SEF_Uint64 getMillsec()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto millseconds = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	SEF_Uint64 value = millseconds.time_since_epoch().count();
	return value;
}

inline static void workSleep(SEF_Uint64 millSec) 
{
	if (millSec < 20)
		return;
	std::this_thread::sleep_for(std::chrono::milliseconds(millSec));
}


ElementWorker::ElementWorker(Element::Element* element)
{
	m_element = element;
	m_stop = false;
}

ElementWorker::~ElementWorker()
{
	m_stop = true;
	LOGFMTT("%s element worker exit\n", m_element->GetElementName().c_str());
}



void ElementWorker::WorkerRunning()
{
	//SEF_Uint64 startTime = getMillsec();
	if (m_element->GetElementEnable() == true && m_stop == false && m_pause == false)
	{
		switch (m_element->GetElementMode())
		{
		case ModeType::Initialize:
			m_element->Initialize();
			break;
		case ModeType::Freeze:
			m_element->Freeze();
			break;
		case ModeType::Run:
			m_element->Run();
			break;
		case ModeType::Shutdown:
			m_element->Shutdown();
			break;
		default:
			m_element->Freeze();
			LOGFMTW("element %s is at error modeType:%d", m_element->GetElementName().c_str(), m_element->GetElementMode());
			break;
		}
		m_element->ModeFrameIncrease();
		m_count++;
	}
}
bool ElementWorker::init()
{
	// TODO element 
	try {
		if (m_isElementInit == false) 
		{
			m_element->Startup();
			m_element->Initialize();
			m_element->SetElementMode(ModeType::Run);
			m_isElementInit = true;
		}
	}
	catch (std::string exp) {
		LOGFMTT("element %s init failed, checkout configure, throw:%s\n", m_element->GetElementName().c_str(), exp.c_str());
		m_isElementInit = false;
	}
	return m_isElementInit;
}



ElementThreadWrapper::ElementThreadWrapper(Element::Element* element, StackTimer& timerStack, SEF_Int32 frequency) :ElementWorker(element), m_timerStack(timerStack),
m_freq(frequency)
{

}

void ElementThreadWrapper::SleepControl()
{
	SEF_Uint64 nowMill = getMillsec();
	if (m_startTime == 0)
	{
		m_startTime = nowMill;
		return;
	}
	else
	{
		SEF_Uint64 m_sleepMill = 0;
		SEF_Uint64 m_durMill = 0;
		//int freq = m_element->GetElementFrequency();
		int freq = m_freq;
		m_durMill = nowMill - m_startTime;
		if (m_count >= freq)
		{
			if (m_durMill < durationMs) 
			{
				m_sleepMill = durationMs - m_durMill;
				//workSleep(m_sleepMill);
				if (m_freqCompensation > 0&& m_sleepMill> durationMs*0.2&& m_numOfSuccess >30) 
				{
					m_freqCompensation--;
				}
			}
			LOGFMTT("%s element worker finish success, reset\n", m_element->GetElementName().c_str());
		}
		else {
			int leftCycle = freq - m_count+ m_freqCompensation;
			if (m_durMill < durationMs) 
			{
				m_sleepMill = (durationMs - m_durMill) / leftCycle;
				workSleep(m_sleepMill);
			}
			else 
			{
				LOGFMTT("%s element worker finish timeout, real frequency %d, reset\n", m_element->GetElementName().c_str(), m_count);
			}

		}
	}	
}

ElementThreadWrapper::~ElementThreadWrapper()
{
	LOGFMTT("element thread wrapper free\n");
	m_timerStack.cancelTimer(this);
	StopThread();
}
bool SEF::Kernel::ElementThreadWrapper::SetModeType(ModeType type)
{
	Element::Element* element = getElement();
	element->SetElementMode(type);
	return true;
}
bool ElementThreadWrapper::Reset()
{
	m_count = 0;
	m_startTime = 0;
	if (m_stop == false && m_isElementInit == true)
	{
		m_timerStack.addTimer(this, durationMs);
	}
	m_pause = false;
	return true;
}


void ElementThreadWrapper::timeoutCB()
{
	//int fre = m_element->GetElementFrequency();
	int fre = m_freq;
	if (m_count < fre)
	{
		LOGFMTT("%s element working timeout, operation is too slow[%d/%d] %f(m_freqCompensation %d)",
			m_element->GetElementName().c_str(), m_count , fre,
			m_count/(fre *1.00), m_freqCompensation);
		m_freqCompensation++;
		m_numOfSuccess = 0;
	}
	else
	{
		LOGFMTT("%s timeout", m_element->GetElementName().c_str());
		isSet = false;
		m_numOfSuccess++;
	}
}

bool SEF::Kernel::ElementThreadWrapper::isInit()
{
	return m_isElementInit;
}

Element* ElementThreadWrapper::getElement()
{
	return m_element;
}

void SEF::Kernel::ElementThreadWrapper::Init()
{
	m_cmd_initElement = true;
	SetModeType(ModeType::Initialize);
}

void ElementThreadWrapper::StartThread()
{
	if (m_warpperInit == false)
	{
		m_thd = std::thread(&ElementThreadWrapper::threadFunction, this);
		m_warpperInit = true;
	}
}

void ElementThreadWrapper::StopThread()
{
	if (m_thd.joinable()&& m_warpperInit == true)
	{
		m_stop = true;
		m_thd.join();
	}
}

void ElementThreadWrapper::Pause(float& ratio)
{
	ratio = m_count / (/*m_element->GetElementFrequency()*/m_freq*1.00);
	m_pause = true;
	bool res = m_timerStack.cancelTimer(this);
	if (res == false)
	{
		LOGFMTT("%s cancel timer when pause failed", m_element->GetElementName().c_str());
	}
	else
	{
		LOGFMTT("%s cancel timer when pause successful", m_element->GetElementName().c_str());
	}
}

void ElementThreadWrapper::threadFunction()
{
	do {
		if (m_cmd_initElement == true&& m_isElementInit == false)
		{
			init();
			m_cmd_initElement = false;
		}
		else if (m_isElementInit == true)
		{
			if (m_pause == true)
			{
				workSleep(20);
				continue;
			}
			if (m_count >= /*m_element->GetElementFrequency()*/m_freq)
			{
				workSleep(20);
				continue;
			}
			WorkerRunning();
			SleepControl();
		}
	} while (m_stop == false);

	LOGFMTT("%s element thread exit\n", m_element->GetElementName().c_str());
}
