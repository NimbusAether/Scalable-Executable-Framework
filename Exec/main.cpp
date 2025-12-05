#include "CentralizationExec.h"
#include "log.h"
#include <assert.h>
#include "UdmMsg.h"
#include "udmRequestQueue.h"

#ifdef VLD_DEBUG
#include "vld.h"
#endif

using namespace SEF::Exec;
using namespace SEF::Log;

void testMsg()
{
	UdmMsg msg;
	string sourceName = "sourceName";
	string topic = "topic";
	string vn = "VariableName";
	string vuint = "VariableUnit";
	string value = "0";
	msg.SetMsgType(SEF::COMMON::ValueType::VALUE_TYPE_BOOL);
	msg.SetSourceName(sourceName);
	msg.SetTimestamp(123456);
	msg.SetTopicName(topic);
	msg.SetVariableName(vn);
	msg.SetVariableUnit(vuint);
	msg.serialize(SEF::COMMON::ValueType::VALUE_TYPE_BOOL, value, value.size(), msg.GetTopicName(), msg.GetVariableName(), msg.GetVariableUnit());

	SEF_Size_t size = 0;
	const void* data = msg.GetBytes(size);
	ByteArray array;
	array.setBytes(data, size);
	UdmMsg msg1;
	msg1.deserialize(array.getBytes(), array.getSize());
}

void testLog()
{
	UdmLogConfig udmLogConfig;
	string path("test.log");
	udmLogConfig.m_level = SEFLogLevel::Trace;
	udmLogConfig.m_path = path;
	UdmLogFactory::setConfig(LogLibType::CLOGGER, &udmLogConfig);
}
void printRequest(std::shared_ptr<Request> req)
{
	SEF_Uint32 count = req->parameterCount();
	for (int i = 0; i<count; i++) {
		ValueType vType = req->getParameterType(i);
		switch (vType) {
		case ValueType::VALUE_TYPE_BOOL: {
			bool value = req->getBool(i);
			LOGDEX("get Request bool:" + to_string(value));
			break;
		}
		case ValueType::VALUE_TYPE_INT16: {
			SEF_Int16 value = req->getInt16(i);
			LOGDEX("get Request int16:" + to_string(value));
			break;
		}
		case ValueType::VALUE_TYPE_INT32: {
			SEF_Int32 value = req->getInt32(i);
			LOGDEX("get Request int32:" + to_string(value));
			break;
		}
		case ValueType::VALUE_TYPE_STRING: {
			SEF_String value = req->getString(i);
			LOGDEX("get Request string:" + value);
			break;
		}
		}
	}
	LOGDEX("request to str:\n" + req->toString());
}

std::shared_ptr<Request>  buildRequest(const std::string & name)
{
	std::shared_ptr<Request> request = std::make_shared<Request>();
	request->setName(name);
	request->setSender("sender");
	request->add(1);
	request->add(2);
	request->add("str");
	request->add("str");
	request->add("str");
	request->add("str");
	request->add("str");
	request->add(false);
	printRequest(request);
	return request;
}

void requestTest()
{
	ByteArray bytes;
	std::shared_ptr<Request> request = buildRequest("requestname0");
	request->serialize(bytes);

	std::shared_ptr<Request> req = std::make_shared<Request>();
	req->deserialize(bytes);
	printRequest(req);
}

void requestQueueTest()
{
	ByteArray bytes;
	RequestQueue queue("queue name");
	queue.addRegisteredRequest("requestname1");
	queue.pushBack(buildRequest("requestname1"));

	queue.addRegisteredRequest("requestname2");
	queue.pushBack(buildRequest("requestname2"));

	queue.addRegisteredRequest("requestname3");
	queue.pushBack(buildRequest("requestname3"));

	queue.addRegisteredRequest("requestname4");
	queue.pushBack(buildRequest("requestname4"));

	queue.serialize(bytes);
	LOGDEX("queue-0:\n"+queue.toString());

	RequestQueue queue1;
	queue1.deserialize(bytes);
	LOGDEX("queue-1:\n" + queue1.toString());
}

void testTimer()
{
	UdmLogConfig udmLogConfig;
	udmLogConfig.m_level = SEFLogLevel::Trace;
	udmLogConfig.m_path = "D:\\Projects\\technology-integration-framework-windows\\sys\\sys\\logs\\exec.log";
	UdmLogFactory::setConfig(LogLibType::CLOGGER, &udmLogConfig);

	StackTimer stackTimer;

	class timerEntryTest:public  timerEntry{
	public:
		timerEntryTest(const string & name) 
		{ 
			LOGFMTD("%s create %p", m_name.c_str(), this);
			m_name = name;
		}
		~timerEntryTest()
		{
			LOGFMTD("%s free %p", m_name.c_str(), this);
		}
		void timeoutCB()
		{
			LOGFMTD("%s timer timeout %p", m_name.c_str(), this);
		}
		string m_name;
	};
	LOGFMTD("=============================test Timer===========================================");
	do {
		list<timerEntryTest*>list;
		LOGFMTD("#######################################################");
		for (long long i = 0; i < 100; i++) 
		{
			timerEntryTest* timer = new timerEntryTest("timeout");
			stackTimer.addTimer(timer, 2000);
			list.push_back(timer);
			_sleep(40);
		}

		for (long long i = 0; i < 100; i++) 
		{
			timerEntryTest* timer = new timerEntryTest("cancel");
			stackTimer.addTimer(timer, 2001);
			bool res = stackTimer.cancelTimer(timer);
			LOGFMTD("cancelAction %s cancel %p ret=%d", timer->m_name.c_str(), timer, res);
			assert(res==true);
			list.push_back(timer);
			_sleep(40);
		}

		for (timerEntryTest* t : list) 
		{
			bool res = stackTimer.cancelTimer(t);
			LOGFMTD("allCancelAction %s cancel %p ret=%d", t->m_name.c_str(),t, res);
			LOGFLUSH();
			delete t;
		}
		LOGFLUSH();
		_sleep(1000);
	} while (1);
}

int main(int argc, char* argv[])
{
//#ifdef VLD_DEBUG
//	VLDGlobalEnable();
//#endif
	//int j = 0;
	//testLog();
	//while (j++<10)
	//{
	//	requestQueueTest();
	//	_sleep(100);
	//}
	//return 0;

	CentralizationExec exec;
	string exec_version = "1.0.0";
	string config_str = "";

	std::cout << "######################################################################" << std::endl;
	std::cout << "#   Welcome to use SEF: Scalable Executable Framework!" << std::endl;
	if (argc == 2) {
		
		if (strcmp(argv[1], "-c") == 0) 
		{
			std::cout << "#   Error: Incorrect arguments." << std::endl;
			std::cout << "#   You did not enter a configuration file." << std::endl;
			std::cout << "#   Please use:" << " -c <Config Json File>" << std::endl;
			std::cout << "######################################################################" << std::endl;
			return 1;
		}
		else if (strcmp(argv[1], "-h") == 0) 
		{
			std::cout << "#   You can use -h|-c|-v arguments as follows:" << std::endl;
			std::cout << "#   -h: Print help informations." << std::endl;
			std::cout << "#   -c <Config Json File>: Run SEF onlywith a configuration file." << std::endl;
			std::cout << "#   -v: Print SEF current version." << std::endl;
			std::cout << "######################################################################" << std::endl;
		}
		else if (strcmp(argv[1], "-v") == 0) 
		{
			std::cout << "#   The current version is " << exec_version << std::endl;
			std::cout << "######################################################################" << std::endl;
		}
		else 
		{
			std::cout << "Error: Incorrect arguments." << std::endl;
			std::cout << "Usage: " << std::endl;
			std::cout << argv[0] << " -c|-h|-v" << std::endl << std::endl;
			std::cout << "######################################################################" << std::endl;
			return 1;
		}
	}
	else if (argc == 3) 
	{
		if (strcmp(argv[1], "-c") == 0)
		{
			config_str = argv[2];
			std::cout << "#   The current version is " << exec_version << std::endl;
			std::cout << "#   The current configuration is " << config_str << std::endl;
			std::cout << "######################################################################" << std::endl;
		}
	}
	else
	{
		std::cout << "#   Error: Incorrect arguments." << std::endl;
		std::cout << "#   Usage: " << std::endl << std::endl;
		std::cout << argv[0] << " -c|-h|-v" << std::endl;
		std::cout << "######################################################################" << std::endl;
		return 1;
	}

	exec.createExecConfig(ConfigType::JSON);
	exec.createExecKernel();

	try
	{
		exec.createExecElementsByConfig(config_str);
	}
	catch (std::string errs)
	{
		std::cout << "check file:[" + config_str + "] error: " + errs << std::endl;
		return 1;
	}

	exec.RunExec();
	int i = 0;
	do {
		std::cout <<"ÇëÊäÈë: 'exit' ÍË³ö³ÌÐò" << std::endl;
		std::string in;
		std::cin >> in;
		if (in.compare("exit")==0)
		{
			break;
		}
	} while (i<60);
	
	return 0;
}
