#include<iostream>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include <immintrin.h>
using namespace std;
const int size[13] = {10,20,30,50,100,200,400,500,800,1000,1300,1600,2000};
float A[2000][2000];
void initial(int N) {
    srand((int)time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = 1 + rand() % 100;
        }
    }
}

void gaosi(int N) {
    for (int k = 0; k < N; k++) {
        __m256 vt = _mm256_set1_ps(A[k][k]);
        int j = k + 1;
        for (; j + 8 <= N; j += 8) {
            __m256 va = _mm256_loadu_ps(&A[k][j]);
            va = _mm256_div_ps(va, vt);
            _mm256_storeu_ps(&A[k][j], va);
        }
        for (; j < N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i = k + 1; i < N; i++) {
            __m256 vaik = _mm256_set1_ps(A[i][k]);
            int t = k + 1;
            for (; t + 8 <= N; t += 8) {
                __m256 vakj = _mm256_loadu_ps(&A[k][t]);
                __m256 vaij = _mm256_loadu_ps(&A[i][t]);
                __m256 vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_storeu_ps(&A[i][t], vaij);
            }
            for (; t < N; t++) {
                A[i][t] -= A[i][k] * A[k][t];
            }
            A[i][k] = 0.0;
        }
    }
}

int main() {
    struct timespec sts,ets;
    for(int t = 0;t < 13;t ++){
        int N = size[t];
        initial(N);
        timespec_get(&sts, TIME_UTC);
        gaosi(N);
        timespec_get(&ets, TIME_UTC);
        time_t dsec=ets.tv_sec-sts.tv_sec;
        long dnsec=ets.tv_nsec-sts.tv_nsec;
        if (dnsec<0){
            dsec--;
            dnsec+=1000000000ll;
        }
        printf ("%ld.%09lds\n",dsec,dnsec);      
    }
    return 0;
}