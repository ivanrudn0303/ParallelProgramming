To generate array to sort:
    rm Filename
    ./GenerateArray.sh Size Filename

To compile:
    g++ -O2 -std=c++17 -o out main.cpp ThreadPool.cpp -lpthread -lrt -fgnu-tm 

To run:
    cat Filename | ./out NumOfThreads
