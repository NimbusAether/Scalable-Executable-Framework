#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "base.h"
namespace SEF
{
    namespace COMMON
    {
        template <typename T>
        class SafeQueue
        {
        public:
            SafeQueue();
            ~SafeQueue();
            SafeQueue(SafeQueue<T>&& other);
            bool push(const T& item);
            bool pop(T& item);
            bool tryPop(T& item);
            void terminate();
            bool isEmpty();
            size_t size();
            //bool operator<(const SafeQueue<T>& other);

        private:
            std::queue<T> mQueue;
            mutable std::mutex mMutex;
            std::condition_variable mCondition;
            bool isTerminate = false;
        };

        template<typename T>
        inline SafeQueue<T>::SafeQueue()
        {

        }

        template<typename T>
        inline SafeQueue<T>::~SafeQueue()
        {
            terminate();
        }

        template<typename T>
        inline SafeQueue<T>::SafeQueue(SafeQueue<T>&& other)
        {
            other.mMutex.lock();
            mQueue = std::move(other.mQueue);
            other.mMutex.unlock();
        }

        template<typename T>
        inline bool SafeQueue<T>::push(const T& item)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (isTerminate == true) 
            {
                return false;
            }
            mQueue.push(item);
            mCondition.notify_one();
            return true;
        }

        template<typename T>
        inline bool SafeQueue<T>::pop(T& item)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            while (mQueue.empty() && isTerminate==false)
            {
                mCondition.wait(lock);
            }
            if (isTerminate == true) 
            {
                return false;
            }
            item = mQueue.front();
            mQueue.pop();
            return true;
        }

        template<typename T>
        inline bool SafeQueue<T>::tryPop(T& item)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mQueue.empty()||isTerminate == true) 
            {
                return false;
            }
            item = mQueue.front();
            mQueue.pop();
            return true;
        }

        template<typename T>
        inline void SafeQueue<T>::terminate()
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if (isTerminate == false) 
            {
                isTerminate = true;
                mCondition.notify_one();
            }
        }

        template<typename T>
        inline bool SafeQueue<T>::isEmpty()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            return mQueue.empty();
        }

        template<typename T>
        inline size_t SafeQueue<T>::size()
        {
            return mQueue.size();
        }
    }
}