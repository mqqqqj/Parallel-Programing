#include<iostream>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
const int size[13] = {10,20,30,50,100,200,400,500,800,1000,1300,1600,2000};
int A[2000][2000];
int R[2000];
void initial(int N) {
    srand((int)time(NULL));
    for (int i = 0; i < N / 3; i++) {
        R[i] = i;
        A[i][i] = 1;
        for (int j = 0; j < N; j++) {
            if (j < i)
                A[i][j] = 0;
            else if (j > i) {
                int flag = rand() % 100;
                if (flag < 50)
                    A[i][j] = 1;
                else
                    A[i][j] = 0;
            }
        }
    }
    for (int i = N / 3; i < N; i++) {
        R[i] = -1;
        for (int j = 0; j < N; j++) {
            int flag = rand() % 100;
            if (flag < 50)
                A[i][j] = 1;
            else
                A[i][j] = 0;
        }
    }
}

int getfirstone(int* a,int N) {
    for (int i = 0; i < N; i++) {
        if (a[i])return i;
    }
    return -999;
}

void gaosi(int N) {
    for (int i = N / 3; i < N; i++) {
        while (getfirstone(A[i],N)!=-999) {
            int lp = getfirstone(A[i],N);
            if (R[lp] != -1) {
                for (int j = 0; j < N; j++) {
                    A[i][j] =A[i][j]^A[R[lp]][j];
                }
            }
            else {
                R[lp] = i;
                break;
            }
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
        // for(int i = 0; i < N;i ++){
        //     for(int j = 0;j < N;j ++){
        //         cout<<A[i][j]<<" ";
        //     }
        //     cout<<endl;
        // }      
    }
    return 0;
}