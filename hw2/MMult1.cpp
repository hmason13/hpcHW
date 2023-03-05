// g++ -std=c++11 -O3 MMult1.cpp && ./a.out
// "compiler clang does not support march native"

#include <stdio.h>
#include <math.h>
// #include <omp.h> 
#include "utils.h"

#define BLOCK_SIZE 16

// Note: matrices are stored in column major order; i.e. the array elements in
// the (m x n) matrix C are stored in the sequence: {C_00, C_10, ..., C_m0,
// C_01, C_11, ..., C_m1, C_02, ..., C_0n, C_1n, ..., C_mn}
void MMult0(long m, long n, long k, double *a, double *b, double *c) {
  for (long p = 0; p < n; p++) {
    for (long j = 0; j < k; j++) {
      for (long i = 0; i < m; i++) {
        double A_ip = a[i+p*m];
        double B_pj = b[p+j*k];
        double C_ij = c[i+j*m];
        C_ij = C_ij + A_ip * B_pj;
        c[i+j*m] = C_ij;
      }
    }
  }
}

void MMult1(long m, long n, long k, double *a, double *b, double *c, double *z) {
  for (long j = 0; j < n; j += BLOCK_SIZE) {
    for (long i = 0; i < k; i+= BLOCK_SIZE) {
      for (int s = 0; s < BLOCK_SIZE*BLOCK_SIZE; s++) z[s] = 0;
      for (long p = 0; p < m; p++) {
        for (int jb = 0; jb < BLOCK_SIZE; jb++) {
          for (int ib = 0; ib < BLOCK_SIZE; ib++) {
            double hold1 = a[(i+ib)+p*m] * b[p+((j+jb)*k)];
            z[ib+(BLOCK_SIZE*jb)] += hold1;
          }
        }
      }
      for (int jb = 0; jb < BLOCK_SIZE; jb++) {
        for (int ib = 0; ib < BLOCK_SIZE; ib++) {
          double hold2 = z[ib+(BLOCK_SIZE*jb)];
          c[(i+ib)+(m*(j+jb))] = hold2;
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  const long PFIRST = 128;
  const long PLAST = 2000;
  const long PINC = 64; // multiple of BLOCK_SIZE

  printf(" Dimension      Time   Gflop/s      GB/s       1-0     Error\n");
  for (long p = PFIRST; p < PLAST; p += PINC) {
    long m = p, n = p, k = p;
    long NREPEATS = 1e6/(m*n*k)+1;
    double* a = (double*) aligned_malloc(m * k * sizeof(double)); // m x k
    double* b = (double*) aligned_malloc(k * n * sizeof(double)); // k x n
    double* c = (double*) aligned_malloc(m * n * sizeof(double)); // m x n
    double* c_ref = (double*) aligned_malloc(m * n * sizeof(double)); // m x n
    // storage for matrix blocks
    double* z = (double*) aligned_malloc(BLOCK_SIZE*BLOCK_SIZE*sizeof(double));

    // Initialize matrices
    for (long i = 0; i < m*k; i++) a[i] = drand48();
    for (long i = 0; i < k*n; i++) b[i] = drand48();
    for (long i = 0; i < m*n; i++) c_ref[i] = 0;
    for (long i = 0; i < m*n; i++) c[i] = 0;

    Timer s;
    s.tic();
    for (long rep = 0; rep < NREPEATS; rep++) { // Compute reference solution
      MMult0(m, n, k, a, b, c_ref);
    }
    float time0 = s.toc();

    Timer t;
    t.tic();
    for (long rep = 0; rep < NREPEATS; rep++) {
      MMult1(m, n, k, a, b, c, z);
    }
    double time = t.toc();
    // 1 add & 1 multiply, p^3 times. Kflop is 1000 flops
    double flops = (NREPEATS*m*n*k*2)/(time*1000*1000*1000);
    // 4 is 3 reads & 1 write. 8 bytes for each double. 1000 kb per 1 mb.
    double bandwidth = (NREPEATS*m*n*k*4*8)/(time*1000*1000*1000);
    float difference = time - time0;

    printf("%10ld %10f %10f %10f %10f", p, time, flops, bandwidth, difference);

    double max_err = 0;
    for (long i = 0; i < m*n; i++) max_err = std::max(max_err, fabs(c[i] - c_ref[i]));
    printf(" %10e\n", max_err);

    aligned_free(a);
    aligned_free(b);
    aligned_free(c);
    aligned_free(z);
  }

  return 0;
}

// * Using MMult0 as a reference, implement MMult1 and try to rearrange loops to
// maximize performance. Measure performance for different loop arrangements and
// try to reason why you get the best performance for a particular order?
//
//
// * You will notice that the performance degrades for larger matrix sizes that
// do not fit in the cache. To improve the performance for larger matrices,
// implement a one level blocking scheme by using BLOCK_SIZE macro as the block
// size. By partitioning big matrices into smaller blocks that fit in the cache
// and multiplying these blocks together at a time, we can reduce the number of
// accesses to main memory. This resolves the main memory bandwidth bottleneck
// for large matrices and improves performance.
//
// NOTE: You can assume that the matrix dimensions are multiples of BLOCK_SIZE.
//
//
// * Experiment with different values for BLOCK_SIZE (use multiples of 4) and
// measure performance.  What is the optimal value for BLOCK_SIZE?
//
//
// * What percentage of the peak FLOP-rate do you achieve with your code?
//
//
// NOTE: Compile your code using the flag -march=native. This tells the compiler
// to generate the best output using the instruction set supported by your CPU
// architecture. Also, try using either of -O2 or -O3 optimization level flags.
// Be aware that -O2 can sometimes generate better output than using -O3 for
// programmer optimized code.
