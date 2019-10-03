
// File main.cc
#include <stdio.h>
#include "blockmatr.h"
#include "FastBlockMatr.hpp"
#include <X86intrin.h>
#include <string>
using namespace std;
int main(int argc, char **argv) {
  const int ay = argc > 1 ? std::stoi(argv[1]) : 1000,
            ax = argc > 2 ? std::stoi(argv[2]) : 900,
            by = ax,
            bx = argc > 3 ? std::stoi(argv[3]) : 1200,
            cy = ay, cx = bx;
  
  blockmatrix<double> a(ay,ax), b(by, bx), c(cy, cx);
  Matrix<double> ma(ay, ax), mb(by, bx);
  
  for (int i = 0; i < ay; i++) {
    for (int j = 0; j < ax; j++) {
      *a(i,j) = i*j;
      ma(i, j) = i*j;
    } 
  }
    
  for (int i = 0; i < by; i++) {
    for (int j = 0; j < bx; j++) {
      *b(i,j) = i*j;
      mb(i, j) = i*j;
    }
  }

  long long start = __rdtsc();
  c.mul(a,b);
  long long end   = __rdtsc();
  double sum = 0;
  for (int i = 0; i < cy; i++) {
    for (int j = 0; j < cx; j++) {
      sum += *c(i,j);
    } 
  }
  printf("sum=%.0lf time=%.6f\n", sum, (end - start)/1000000000.);

  sum = 0.0;
  start = __rdtsc();
  auto mc = ma * mb;
  end = __rdtsc();
    for (int i = 0; i < cy; i++)
        for (int j = 0; j < cx; j++)
            sum += mc(i,j);
  printf("sum=%.0lf time=%.6f\n", sum, (end - start)/1000000000.);
}
