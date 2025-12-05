#pragma once
#include <iostream>
#include <ctime>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <thread>
#include <unordered_map>
#include "base.h"
namespace SEF
{
    namespace COMMON
    {
        class StackTimerContent;

        class UDM_COMMON_API timerEntry {
        public:
            timerEntry(): isSet(false){
            }
            virtual ~timerEntry() {

            }
            virtual void timeoutCB() = 0;
        public:
            bool isSet;
        };

        class UDM_COMMON_API StackTimer {
        public:
            StackTimer();
            ~StackTimer();
            void stop();
            void addTimer(timerEntry* obj, int delayMilliseconds);
            bool cancelTimer(timerEntry* obj);
        private:
            StackTimerContent* m_ctx = nullptr;
        private:
            void stackCycleRun();
        };
    }
}



