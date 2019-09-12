#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
   int step = argc > 1 ? atoi(argv[1]) : 1;
   int size = argc > 2 ? atoi(argv[2]) : 1024*1024*512;
   int *a = new int[size];
   for (int i = 0; i < size; i++) {
      if ((i & 63) == 0) a[i] = rand();
      else		 a[i] = i-a[i-1];
   }
   long long sum = 0;
   clock_t start = clock();
   for (int i = 0; i < size; i += step ) {
      sum += a[i];
   }
   clock_t end = clock();
   printf("sum=%lld time=%.4f\n", sum, (double)(end - start) / CLOCKS_PER_SEC);
}

