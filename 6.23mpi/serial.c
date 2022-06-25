#include <stdio.h>
#include<time.h>
#include <stdlib.h>
const int size[13] = {10,20,30,50,100,200,400,500,800,1000,1300,1600,2000};
float A[2000][2000];

void initial(int N){
    srand((int)time(NULL));
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < N;j ++){
            A[i][j] = i+j+rand() % 100;
        }
    }
}

void gaosiSerial(int N){
    for(int k = 0;k < N;k ++){
        for(int j = k + 1;j < N;j ++){
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

void showres(int N){
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < N;j ++){
            printf(" %f",A[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    struct timespec sts,ets;
    for(int t = 0;t < 13;t ++){
        int N = size[t];
        initial(N);
        timespec_get(&sts, TIME_UTC);
        gaosiSerial(N);
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