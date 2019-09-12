#include <thread>


int main()
{
    auto t = std::thread([]{});
    t.join();
    return 0;
}
