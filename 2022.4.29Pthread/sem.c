//静态线程+信号量同步版本
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
const int size[13] = {10,20,30,50,100,200,400,500,800,1000,1300,1600,2000};
float A[2000][2000];

#define NUM_THREADS 2

sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];
sem_t sem_workerend[NUM_THREADS];

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
        sem_wait(&sem_workerstart[t_id]);
        for(int i = k + 1 + t_id;i < N ;i += NUM_THREADS){
            for(int j = k + 1;j < N;j ++){
                A[i][j] -= A[i][k] * A[k][j];
            }
            A[i][k] = 0.0;
        }
    sem_post(&sem_main);
    sem_wait(&sem_workerend[t_id]);
    }
    pthread_exit(NULL);
}

int main(void){
    struct timespec sts,ets;
    for(int t = 0;t < 13;t ++){
        int N = size[t];
        initial(N);
        timespec_get(&sts, TIME_UTC);
        sem_init(&sem_main,0,0);
        for(int i = 0;i < NUM_THREADS;i ++){
            sem_init(&sem_workerstart[i],0,0);
            sem_init(&sem_workerend[i],0,0);
        }
        pthread_t handles[NUM_THREADS];
        threadParam_t param[NUM_THREADS];
        for(int i = 0;i < NUM_THREADS;i ++){
            param[i].t_id = i;
            param[i].t_n = N;
            pthread_create(&handles[i],NULL,gaosi,(void*)&param[i]);
        }
        for(int k = 0;k < N; k ++){    
            for(int j = k + 1;j < N;j ++){
                A[k][j] = A[k][j]/A[k][k];
            }
            A[k][k] = 1.0;
            for(int t_id = 0;t_id < NUM_THREADS;++t_id){
                sem_post(&sem_workerstart[t_id]);
            }
            for(int t_id = 0; t_id < NUM_THREADS; ++t_id){
                sem_wait(&sem_main);
            }
            for(int t_id = 0; t_id < NUM_THREADS; ++t_id){
                sem_post(&sem_workerend[t_id]);
            }
        }
        for(int t_id = 0; t_id < NUM_THREADS; t_id++){
            pthread_join(handles[t_id],NULL);
        }
        sem_destroy(&sem_main);
        for(int t_id = 0;t_id < NUM_THREADS;++t_id){
            sem_destroy(&sem_workerstart[t_id]);
            sem_destroy(&sem_workerend[t_id]);
        }
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