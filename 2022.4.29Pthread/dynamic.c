#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
//using namespace std;
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

typedef struct{
    int k;
    int t_id;
    int t_n;
}threadParam_t;

void* gaosi(void* param){
    threadParam_t*p=(threadParam_t*)param;
    int k =p->k;
    int t_id = p->t_id;
    int N = p->t_n;
    int i = k + t_id + 1;
    for(int j = k + 1;j < N;j ++){
        A[i][j] -= A[i][k] * A[k][j]; 
    }
    A[i][k] = 0;
    pthread_exit(NULL);
}
int main(){
    struct timespec sts,ets;
    for(int t = 0;t < 13;t ++){
        int n = size[t];
        initial(n);
        timespec_get(&sts, TIME_UTC);
        for(int k = 0;k < n;k ++){
            for(int j = k + 1;j < n;j ++ ){
                A[k][j] = A[k][j]/A[k][k];
            }
            A[k][k] = 1.0;
            //创建线程
            int worker_count = n-1-k;
            pthread_t* handles = malloc(worker_count*sizeof(pthread_t));
            threadParam_t* param = malloc(worker_count*sizeof(threadParam_t)); 
            for(int t_id = 0;t_id < worker_count;t_id ++){
                param[t_id].k=k;
                param[t_id].t_id = t_id;
                param[t_id].t_n = n;
            }
            for(int t_id = 0;t_id < worker_count;t_id ++){
                pthread_create(&handles[t_id],NULL,gaosi,(void*)&param[t_id]);
            }
            for(int t_id = 0;t_id < worker_count;t_id ++){
                pthread_join(handles[t_id],NULL);
            }
        }
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
