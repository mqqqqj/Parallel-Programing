#include<iostream>
#include<time.h>
#include<stdio.h>
#include <arm_neon.h>
#include<stdlib.h>
using namespace std;
const int size[13] = {10,20,30,50,100,200,400,500,800,1000,1300,1600,2000};
float A[2000][2000];
void initial(int N){
    srand((int)time(NULL));
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < N;j ++){
            A[i][j] = 1+rand() % 100;
        }
    }
}

void gaosi(int N){
    for(int k = 0;k < N;k ++){
        float32x4_t vt=vdupq_n_f32(A[k][k]); // 把数值拷贝四份到寄存器中
        int j = k + 1;
        for( ;j + 4 <= N;j += 4){
            float32x4_t va = vld1q_f32(&A[k][j]);
            va = vdivq_f32(va, vt);
            vst1q_f32(&A[k][j], va);
        }
        for(;j < N ;j ++){
            A[k][j] = A[k][j]/A[k][k];
        }
        A[k][k] = 1.0;
        for(int i = k + 1;i < N;i ++){
            for(int j = k + 1;j < N;j ++){
                A[i][j] -= A[i][k] * A[k][j]; 
            }
            A[i][k] = 0;
        }
    }
}
int main(){
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