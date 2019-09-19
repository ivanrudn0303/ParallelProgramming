#include "ThreadPool.h"

#include <algorithm>
#include <condition_variable>
#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


std::mutex m;
std::condition_variable c;

struct Input
{
    LockFree::ThreadPoolAsync& Pool;
    size_t Threads;
    std::vector<size_t>::iterator First;
    std::vector<size_t>::iterator Last;
    std::atomic<bool>* Done;
};

void Sorter(void* aParams)
{
    Input* sArgs = reinterpret_cast<Input*>(aParams);
    if (sArgs->Last - sArgs->First > 1)
    {
        if (sArgs->Threads > 1)
        {
            auto sMiddle = sArgs->First + (sArgs->Last - sArgs->First) / 2;
            std::atomic<bool> sReady0(false);
            std::atomic<bool> sReady1(false);
            Input sFirst = {sArgs->Pool, sArgs->Threads / 2, sArgs->First, sMiddle, &sReady0};
            Input sSecond = {sArgs->Pool, sArgs->Threads - sArgs->Threads / 2, sMiddle, sArgs->Last, &sReady1};
            if (sArgs->Pool.Do(Sorter, &sFirst))
            {
                std::cout << "run thread" << std::endl;
                Sorter(&sSecond);
                {
                    std::unique_lock<std::mutex> lk(m);
                    c.wait(lk, [&sReady0]{return sReady0.load();});
                }
                std::inplace_merge(sArgs->First, sMiddle, sArgs->Last);
            }
            else
                std::sort(sArgs->First, sArgs->Last);
        }
        else
            std::sort(sArgs->First, sArgs->Last);
    }
    sArgs->Done->store(true);
    c.notify_one();
}

int main(int argc, char* argv[])
{
    size_t sThreads = std::stoul(argv[1], nullptr, 0);
    std::vector<size_t> sArray;

    for (std::string line; std::getline(std::cin, line);) {
        sArray.push_back(std::stoul(line, nullptr, 0));
    }

    LockFree::ThreadPoolAsync sPool(sThreads);
    std::atomic<bool> sReady(false);
    Input sParams = {sPool, sThreads, sArray.begin(), sArray.end(), &sReady};
    std::chrono::high_resolution_clock sClock;
    auto sStart = sClock.now();
    std::cout << "run thread" << std::endl;
    Sorter(&sParams);
    //for (const auto& el: sArray)
    //    std::cout << el << std::endl;
    auto sStop = sClock.now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(sStop - sStart).count() << std::endl;
    return 0;
}
