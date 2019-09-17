To generate array to sort:
    rm Filename
    ./Generate Size Filename

To compile:
    g++ -O2 -std=c++17 -o out main.cpp ThreadPool.cpp -lpthread

To run:
   cat Filename | ./out NumOfThreads
