#pragma once
#include "MultiNodesKernel.h"
#include "Element.h"
#include "Group.h"
#include <thread>
#include <mutex>
#include <map>
#include "udmTimer.h"

using namespace SEF::Element;
using namespace SEF::COMMON;
namespace SEF
{
	namespace Kernel
	{
		struct elementStat {
			SEF_String elementName;
			SEF_Int	status;
			SEF_Int	successedCount;
			SEF_Int	failedCount;
		};

		class elementStatusPublisher {
		public:
			virtual void countPublisher(std::vector<elementStat> &vec) = 0;
		};

		class UDM_EXPORT_API ElementWorker:public timerEntry {
		public:
			ElementWorker(Element::Element* element);
			virtual ~ElementWorker();
			void WorkerRunning();
			bool init();

		protected:
			Element::Element* m_element = nullptr;
			int			m_count = 0;
			SEF_Uint64	m_startTime = 0;
			bool m_isElementInit = false;
			bool m_stop = false;
			bool m_pause = true;

		};
		class UDM_EXPORT_API ElementThreadWrapper: public ElementWorker {
		public:
			ElementThreadWrapper(Element::Element* element, StackTimer &timerStack, SEF_Int32 frequency);
			~ElementThreadWrapper();
			bool SetModeType(ModeType type);

			void Init();
			void StartThread();
			void StopThread();
			void Pause(float &ratio);
			bool Reset();
			bool isInit();
			Element::Element* getElement();
			/*定时器触发，剩余任务清除，进入新一轮mode*/

			void timeoutCB() override;

		private:
			void threadFunction();
			void SleepControl();
			std::thread m_thd;
			std::mutex m_mutex;
			std::condition_variable m_cv;
			bool m_warpperInit = false;
			bool m_cmd_initElement = false;
			StackTimer &m_timerStack;
			int m_freqCompensation = 5;
			SEF_Int32 m_freq;
			COMMON::SEF_Uint64 durationMs = 1000;
			int m_numOfSuccess = 0;
		};

		class UDM_EXPORT_API CentralizationKernel : public MultiNodesKernel, public timerEntry
		{
		public:
			CentralizationKernel();
			virtual ~CentralizationKernel();
			bool AddElementsGroup(Group *group);
			bool AddElement(Element::Element* element);
			vector<Group*> GetElementsGroup();
			vector<Element::Element*> GetElementsNameByGroupName(string elementsGroupName);
			void timeoutCB()override;
			void ExecuteKernel() override;
			bool ElementStatControl(const string& elementName, SEF::Kernel::ModeType modeType);
			void setElemeltPublisherCb(elementStatusPublisher* cb);
		private:
			/*
			* 初始化所有element，调用element的Startup Initialize Freeze 最终保持Run状态，
			* 按照配置频率进行仿真，等待控制命令
			*/
			bool initElements();
			Element::Element* FindElement(const string& elementName);
			bool PublishOutputs(const string &elementName);
			bool SubscribeInputs(const string& elementName);
			bool Startup(const string& elementName);
			bool Initialize(const string& elementName);
			bool Freeze(const string& elementName);
			bool Run(const string& elementName);
			bool Shutdown(const string& elementName);


			ElementThreadWrapper* FindWrapper(const string& elementName);
		protected:
			vector<Group*> processes;
			vector<Group*> elements_group;
			vector<Element::Element*> elements;

			vector<Element::Element*> m_allElements;
			map<string, ElementThreadWrapper *> m_threadWrappers;
			/*TimerQueue m_timerQueue;*/
			bool m_isRunning = false;

			StackTimer m_stackTimer;
			int m_currentCount;
		private:
			elementStatusPublisher* m_statusPublisher;
		};
	}
}