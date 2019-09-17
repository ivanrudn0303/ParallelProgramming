#include "ThreadPool.h"


static void ExitForStopAll(void*)
{
    return;
}

LockFree::ThreadPoolSync::ThreadPoolSync(size_t aThreads): m_ThreadListData(aThreads), m_Stop(false), m_IdleThreads(nullptr)
{
    for (size_t i = 0; i < m_ThreadListData.size(); ++i)
    {
        m_ThreadListData[i].Args = nullptr;
        m_ThreadListData[i].Caller = nullptr;
        m_ThreadListData[i].Next = nullptr;
        m_ThreadListData[i].Thread = std::thread([this](size_t idx)
            {
                while (!m_Stop.load())
                {
                    PushForward(&m_ThreadListData[idx]);
                    while (nullptr == m_ThreadListData[idx].Caller.load());
                    m_ThreadListData[idx].Caller.load()(m_ThreadListData[idx].Args);
                    m_ThreadListData[idx].Caller.store(nullptr);
                }
            }, i);
    }
}

void LockFree::ThreadPoolSync::PushForward(LockFree::ThreadPoolSync::ThreadNode* aIdleThread)
{
    do
    {
        aIdleThread->Next = m_IdleThreads.load();
    } while (!std::atomic_compare_exchange_weak(&m_IdleThreads, &aIdleThread->Next, aIdleThread));
}

LockFree::ThreadPoolSync::ThreadNode* LockFree::ThreadPoolSync::PopForward()
{
    ThreadNode* sHead;
    do
    {
        sHead = m_IdleThreads.load();
        if (nullptr == sHead)
            return nullptr;
    } while (!std::atomic_compare_exchange_weak(&m_IdleThreads, &sHead, sHead->Next));
    return sHead;
}

LockFree::ThreadPoolSync::~ThreadPoolSync()
{
    StopAll();
    for (size_t i = 0; i < m_ThreadListData.size(); ++i)
        m_ThreadListData[i].Thread.join();
}

void LockFree::ThreadPoolSync::StopAll()
{
    m_Stop.store(true);
    for (size_t i = 0; i < m_ThreadListData.size(); ++i)
        m_ThreadListData[i].Caller.store(ExitForStopAll);
}

void LockFree::ThreadPoolSync::Do(TaskType aTask, void* aArgs)
{
    if (m_Stop.load())
        return;
    ThreadNode* sIdle;
    while (nullptr == (sIdle = PopForward()));
    TaskType sTmp = nullptr;
    sIdle->Args = aArgs;
    std::atomic_compare_exchange_weak(&sIdle->Caller, &sTmp, aTask);
}
