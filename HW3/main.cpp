#include "SkipList.hpp"

#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;

void f(int num, int idx, SkipList<int>* skip)
{
    for (int i = idx; i < num * 1000; i += num)
    {
        if (i == 0)
            continue;
 //       m.lock();
        skip->insert(i, i * 3);
   //     m.unlock();
    }
}

void f2(int num, int idx, SkipList<int>* skip)
{
    int sRes;
    for (int i = idx; i < num * 1000; i += num)
    {
        if (i == 0)
            continue;
        skip->find(i, sRes);
        if (sRes != 3 * i)
            std::cout << "bad res = " << sRes << " i = " << i << std::endl;
    }
}

void f3(int num, int idx, SkipList<int>* skip)
{
    for (int i = idx; i < num * 1000; i += num)
    {
        if (i == 0)
            continue;
        skip->remove(i);
//        std::cout << "deleted" << i << std::endl;
    }
}

int main()
{
    SkipList<int> sTest;
    std::thread t1(f, 4, 0, &sTest);
    std::thread t2(f, 4, 1, &sTest);
    std::thread t3(f, 4, 2, &sTest);
    std::thread t4(f, 4, 3, &sTest);

    std::cout << "here1\n";

    int sRes;
//    for (int i = 0; i < 4 * 1000; ++i)
//    {
//        while (!sTest.find(i, sRes));
//        if (sRes != i * 3)
//            std::cout << "sRes " << sRes << std::endl;
//    }
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    sTest.Check();
//    sTest.remove(1);
    std::cout << "here2\n";
    std::thread t11(f2, 4, 0, &sTest);
    std::thread t12(f2, 4, 1, &sTest);
    std::thread t13(f2, 4, 2, &sTest);
    std::thread t14(f2, 4, 3, &sTest);

    t11.join();
    t12.join();
    t13.join();
    t14.join();
    sTest.Check();
    return 0;
}
