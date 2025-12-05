#pragma once
#include "Mode.h"
#include "base.h"
#include "RequestQueueIntf.h"
#include <string>

using namespace SEF::Kernel;

using std::string;

namespace SEF {
	namespace Element {
		class UDM_EXPORT_API Element:public RequestQueueIntf
		{
		public:
			Element();
			Element(const string& elementName);
			virtual ~Element();
			void SetElementConfig(const string& elementName, int elementFrequency, bool elementEnabled, int startFrame, const string& typeName);
			void SetElementConfig(int elementFrequency, bool elementEnabled, int startFrame, const string& typeName);
			string GetElementName();
			int GetElementFrequency();
			string GetTypeName();
			ModeType GetElementMode();
			bool GetElementEnable();
			bool SetElementMode(ModeType nextMode);
			void ModeFrameIncrease();
		/*protected:*/
			/** Method called during startup mode. */
			virtual bool Startup();

			/** Method called during the publish minor mode during the startup mode. */
			virtual bool PublishOutputs();

			/** Method called during the subscribe minor mode during the startup mode.
			* It should be noted that all simulation elements publish their outputs prior
			* to the subscription minor mode.
			*/
			virtual bool SubscribeInputs();

			/** Method called during the intialize mode. */
			virtual bool Initialize();

			/** Method called during the freeze mode. */
			virtual bool Freeze();

			/** Method called during the run mode. */
			virtual bool Run();

			/** Method called during the shutdown mode. */
			virtual bool Shutdown();
		protected:
			/**
			* Returns if this is the first frame of the current mode.
			*/
			bool isModeFirstFrame() const;

		protected:
			std::string element_name;
			int element_frequency;
			bool element_enabled;
			int start_frame;
			std::string type_name;
			ModeType element_mode;
		private:
			COMMON::SEF_Uint64 m_currentFrameIndex;
		};
	}
}