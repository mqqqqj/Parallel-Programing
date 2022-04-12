#include<iostream>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include <xmmintrin.h>
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
        __m128 vt = _mm_set_ps1(A[k][k]);
        int j = k + 1;
        int off = j % 4;
        for(int i = 0;i < 4 - off;i ++){
            A[k][j+i]/=A[k][k];
        }
        j = j + 4 - off;
        for (; j + 4 <= N; j += 4) {
            __m128 va = _mm_load_ps(&A[k][j]);
            va = _mm_div_ps(va, vt);
            _mm_storeu_ps(&A[k][j], va);
        }
        A[k][k] = 1.0;
        for (int i = k + 1; i < N; i++) {
            __m128 vaik = _mm_set_ps1(A[i][k]);
            int t = k + 1;
            int off1 = t % 4;
            for(int p = 0;p < 4 - off1;p ++){
                A[i][t+p] -= A[i][k] * A[k][t+p];
            }
            t = t + 4 - off1;
            for (; t + 4 <= N; t += 4) {
                __m128 vakj = _mm_load_ps(&A[k][t]);
                __m128 vaij = _mm_load_ps(&A[i][t]);
                __m128 vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_store_ps(&A[i][t], vaij);
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