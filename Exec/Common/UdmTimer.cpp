#include "UdmTimer.h"
#include "log.h"
using namespace SEF::COMMON;

class Timer {
public:
    timerEntry* obj;
    std::chrono::time_point<std::chrono::steady_clock> expirationTime;

    Timer(timerEntry* objPtr, int delayMilliseconds);
    ~Timer();
    bool operator<(const Timer& other) const;
    int getDelayMs();
    void execute();
    void setCancel();
    bool isCanceled();
private:
    int delayMs;
    bool m_isCanceled;
};

Timer::Timer(timerEntry* objPtr, int delayMilliseconds) : obj(objPtr) , delayMs(delayMilliseconds), m_isCanceled(false)
{
    expirationTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMilliseconds);
}
Timer::~Timer()
{
    m_isCanceled = true;
}
bool Timer::operator<(const Timer& other) const 
{
    if (m_isCanceled&&other.m_isCanceled == false) 
    {
        return false;
    }
    else if(!m_isCanceled && other.m_isCanceled)
    {
        return true;
    }
    else 
    {
        return expirationTime > other.expirationTime;
    }
}

int Timer::getDelayMs()
{
    return delayMs;
}

void Timer::execute()
{
    if (m_isCanceled == false) 
    {
        obj->timeoutCB();
        m_isCanceled = true;
    }
}

void Timer::setCancel()
{
    m_isCanceled = true;
}

bool Timer::isCanceled()
{
    return m_isCanceled;
}

/**************************************************/

class SEF::COMMON::StackTimerContent {
public:
    StackTimerContent()
    {

    }
    ~StackTimerContent()
    {

    }
public:
    std::priority_queue<std::shared_ptr<Timer>>timers;
    std::unordered_map<timerEntry*, std::shared_ptr<Timer>> timerMap;
    std::thread timerThread;
    std::mutex mutex;
    std::condition_variable condition;
    bool is_running = true;
};

StackTimer::StackTimer()
{
    m_ctx = new StackTimerContent();
    m_ctx->timerThread = std::thread(&StackTimer::stackCycleRun, this);
}
StackTimer::~StackTimer()
{
    stop();
    if (m_ctx->timerThread.joinable())
    {
        m_ctx->timerThread.join();
    }
    delete m_ctx;
}
void StackTimer::stop()
{
    std::unique_lock<std::mutex> lock(m_ctx->mutex);
    if (m_ctx->is_running == true)
    {
        m_ctx->is_running = false;
        m_ctx->condition.notify_one();
    }
}
void StackTimer::addTimer(timerEntry* obj, int delayMilliseconds)
{
    std::unique_lock<std::mutex> lock(m_ctx->mutex);
    if (m_ctx->is_running == false)
    {
        return;
    }

    std::shared_ptr<Timer> timer = std::make_shared<Timer>(obj, delayMilliseconds);
    m_ctx->timers.push(timer);
    m_ctx->timerMap.insert(std::make_pair(obj, timer));
    m_ctx->condition.notify_one();
}

bool StackTimer::cancelTimer(timerEntry* obj)
{
    std::unique_lock<std::mutex> lock(m_ctx->mutex);

    auto it = m_ctx->timerMap.find(obj);
    if (it != m_ctx->timerMap.end())
    {
        std::shared_ptr<Timer> cancelTimer = it->second;
        cancelTimer->setCancel();
        m_ctx->timerMap.erase(it);
        return true;
    }
    return false;
}

void StackTimer::stackCycleRun() 
{
    while (m_ctx->is_running == true)
    {
        std::unique_lock<std::mutex> lock(m_ctx->mutex);
        auto currentTime = std::chrono::steady_clock::now();
        if (!m_ctx->timers.empty())
        {
            std::shared_ptr<Timer> timer = m_ctx->timers.top();
            if (timer->isCanceled() == true) 
            {
                m_ctx->timers.pop();
            }
            else if (timer->expirationTime > currentTime)
            {
                m_ctx->condition.wait_until(lock, timer->expirationTime);
                //lock.unlock();
                //std::chrono::milliseconds millValue = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - timer->expirationTime);
                //std::this_thread::sleep_for(millValue);
            }
            else 
            {
                m_ctx->timers.pop();
                m_ctx->timerMap.erase(timer->obj);
                lock.unlock();
                timer->execute();
            }
        }
        else 
        {
            m_ctx->condition.wait(lock);
            //_sleep(10);
        }
    }
    LOGDEX("timer core exit");
}






