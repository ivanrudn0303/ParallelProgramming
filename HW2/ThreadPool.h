#pragma once

#include "Queue.h"

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <utility>
#include <vector>


namespace LockFree
{
    class ThreadPoolAsync
    {
        using TaskType = void (*)(void*);

        sem_t m_Lock;
        std::vector<std::thread> m_Threads;
        Queue<std::pair<TaskType, void*>> m_Tasks;
        std::atomic<bool> m_Stop;

        void StopAll();
    public:
        ThreadPoolAsync(size_t aThreads);
        ~ThreadPoolAsync();
        bool Do(TaskType aTask, void* aArgs);
    };

    class ThreadPoolSync
    {
        using TaskType = void (*)(void*);
        struct ThreadNode
        {
            std::thread Thread;
            std::atomic<TaskType> Caller;
            void* Args;
            ThreadNode* Next;
        };

        std::atomic<ThreadNode*> m_IdleThreads;
        std::vector<ThreadNode> m_ThreadListData;
        std::atomic<bool> m_Stop;

        void StopAll();
        void PushForward(ThreadNode* aIdleThread);
        ThreadNode* PopForward();
    public:
        ThreadPoolSync(size_t aThreads);
        ~ThreadPoolSync();
        void Do(TaskType aTask, void* aArgs);
    };
}
