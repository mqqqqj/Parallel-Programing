//静态线程+信号量同步版本
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <xmmintrin.h>
const int size[13] = {10,20,30,50,100,200,400,500,800,1000,1300,1600,2000};
float A[2000][2000];

#define NUM_THREADS 4

pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;

void initial(int N){
    srand((int)time(NULL));
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < N;j ++){
            A[i][j] = 1+rand() % 100;
        }
    }
}

typedef struct{
    int t_id;
    int t_n;
}threadParam_t;


void* gaosi(void*param){
    threadParam_t*p = (threadParam_t*)param;
    int t_id = p->t_id;
    int N = p->t_n;
    for(int k = 0;k < N;k ++){
        if(t_id == 0){
            __m128 vt = _mm_set_ps1(A[k][k]);
            int j = k + 1;
            for (; j + 4 <= N; j += 4) {
                __m128 va = _mm_loadu_ps(&A[k][j]);
                va = _mm_div_ps(va, vt);
                _mm_storeu_ps(&A[k][j], va);
            }
            for (; j < N; j++) {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1.0;
        }
        pthread_barrier_wait(&barrier_Division);
        for(int i = k + 1 + t_id;i < N ;i += NUM_THREADS){
            __m128 vaik = _mm_set_ps1(A[i][k]);
            int t = k + 1;
            for (; t + 4 <= N; t += 4) {
                __m128 vakj = _mm_loadu_ps(&A[k][t]);
                __m128 vaij = _mm_loadu_ps(&A[i][t]);
                __m128 vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_storeu_ps(&A[i][t], vaij);
            }
            for (; t < N; t++) {
                A[i][t] -= A[i][k] * A[k][t];
            }
            A[i][k] = 0.0;
        }
        pthread_barrier_wait(&barrier_Elimination);
    }
    pthread_exit(NULL);
}

int main(void){
    struct timespec sts,ets;
    for(int t = 0;t < 13;t ++){
        int N = size[t];
        initial(N);
        timespec_get(&sts, TIME_UTC);
        pthread_barrier_init(&barrier_Division,NULL,NUM_THREADS);
        pthread_barrier_init(&barrier_Elimination,NULL,NUM_THREADS);
        pthread_t handles[NUM_THREADS];
        threadParam_t param[NUM_THREADS];
        for(int i = 0;i < NUM_THREADS;i ++){
            param[i].t_id = i;
            param[i].t_n = N;
            pthread_create(&handles[i],NULL,gaosi,(void*)&param[i]);
        }
        for(int i = 0;i < NUM_THREADS;i ++){
            pthread_join(handles[i],NULL);
        }
        pthread_barrier_destroy(&barrier_Division);
        pthread_barrier_destroy(&barrier_Elimination);
        timespec_get(&ets, TIME_UTC);
        time_t dsec=ets.tv_sec-sts.tv_sec;
        long dnsec=ets.tv_nsec-sts.tv_nsec;
        if (dnsec<0){
            dsec--;
            dnsec+=1000000000ll;
        }
        printf ("%ld.%09ld\n",dsec,dnsec);      
    }
    return 0;
}