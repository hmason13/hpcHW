// g++ -std=c++11 -O3 p4Pipeline.cpp && ./a.out

#include <stdio.h>
#include <math.h>
#include "utils.h"


float innerP0(int length, float *v1, float *v2) {
  float result = 0;
  
  for (int i=0; i<length; i++) {
    result += v1[i] * v2[i];    
  }
  
  return result;
}


float innerP1(int length, float *v1, float *v2) {
  float result1 = 0;
  float result2 = 0;
  
  for (int i=0; i < (length/2); i++) {
    result1 += v1[2*i]   *   v2[2*i]; 
    result2 += v1[(2*i)+1] * v2[(2*i)+1];
  }

  return (result1 + result2);
}


float innerP2(int length, float *v1, float *v2) {
  float result1 = 0;
  float result2 = 0;
  
  for (int i=0; i < length/2; i++) {
    result1 += *(v1)   *   *(v2); 
    result2 += *(v1+1) * *(v2+1);
    v1 += 2;
    v2 += 2;
  }
  
  return (result1 + result2);
}


float innerP3(int length, float *v1, float *v2) {
  float result1 = 0;
  float result2 = 0;
  float temp1 = 0;
  float temp2 = 0;
  
  for (int i=0; i < length/2; i++) {
    temp1 = *(v1)   *   *(v2); 
    temp2 = *(v1+1) * *(v2+1);
    
    result1 += temp1;
    result2 += temp2;
    
    v1 += 2;
    v2 += 2;
  }
  
  return (result1 + result2);
}


float innerP4(int length, float *v1, float *v2) {
  float result1 = 0;
  float result2 = 0;
  float temp1 = 0;
  float temp2 = 0;
  
  for (int i=0; i < length/2; i++) {
    result1 += temp1;
    temp1   = *(v1)   *   *(v2); 
    
    result2 += temp2;
    temp2   = *(v1+1) * *(v2+1);
    
    v1 += 2;
    v2 += 2;
  }
  return (result1 + result2 + temp1 + temp2);
}


int main(int argc, char** argv) {
  const int vectorLength = 1e7;
  const int nRepeats = 1e2;
  
  printf("run #    Time     Result  \n");
  
  // allocate memory for vectors
  float *v1 = (float*) aligned_malloc(vectorLength*sizeof(float));
  float *v2 = (float*) aligned_malloc(vectorLength*sizeof(float));
  
  // initialize the vectors
  for (int i=0; i < vectorLength; i++) v1[i] = drand48();
  for (int i=0; i < vectorLength; i++) v2[i] = drand48();
  
  Timer t0;
  t0.tic();
  float r0 = 0;
  for (int repeat = 0; repeat < nRepeats; repeat++) {
    r0 += innerP0(vectorLength, v1, v2);
  }
  float time0 = t0.toc();
  printf("%10s %10f %10f", "\n N 0", time0, r0);

  Timer t1;
  t1.tic();
  float r1 = 0;
  for (int repeat = 0; repeat < nRepeats; repeat++) {
    r1 += innerP1(vectorLength, v1, v2);
  }
  float time1 = t1.toc();
  printf("%10s %10f %10f", "\n N 1", time1, r1);

  Timer t2;
  t2.tic();
  float r2 = 0;
  for (int repeat = 0; repeat < nRepeats; repeat++) {
    r2 += innerP2(vectorLength, v1, v2);
  }
  float time2 = t2.toc();
  printf("%10s %10f %10f", "\n N 2", time2, r2);
  
  Timer t3;
  t3.tic();
  float r3 = 0;
  for (int repeat = 0; repeat < nRepeats; repeat++) {
    r3 += innerP3(vectorLength, v1, v2);
  }
  float time3 = t1.toc();
  printf("%10s %10f %10f", "\n N 3", time3, r3);

  Timer t4;
  t4.tic();
  float r4 = 0;
  for (int repeat = 0; repeat < nRepeats; repeat++) {
    r4 += innerP4(vectorLength, v1, v2);
  }
  float time4 = t4.toc();
  printf("%10s %10f %10f", "\n N 4", time4, r4);
  
  aligned_free(v1);
  aligned_free(v2);

  return 0;
}
