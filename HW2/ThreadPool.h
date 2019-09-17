#pragma once

#include <atomic>
#include <cstdio>
#include <functional>
#include <thread>
#include <vector>


namespace LockFree
{
//    class ThreadPool
//    {
//        using TaskType = std::function<void()>;
//        std::vector<std::thread> m_Threads;
//        Queue<TaskType> m_Tasks;
//    public:
//        ThreadPool(size_t aThreads);
//        ~ThreadPool();
//        void Do(const TaskType& aTask);
//    };

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
