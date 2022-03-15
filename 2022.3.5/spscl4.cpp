#include<iostream>
#include<sys/time.h>
using namespace std;
const int size[14]={2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384};
int array[16384];

void func(int* a,int n)
{
    for (int m = n; m > 1; m /= 2) 
        for (int i = 0; i < m / 2; i++)
            a[ i ] = a[i * 2] + a[i * 2 + 1] ;
}

int main(){
    srand(time(NULL));
    for(int i = 0;i < 16384;i ++){
        array[i] = rand()%1000;
    }
    timeval t1,t2;
    for(int i = 0;i < 14;i ++){
        int round = 0;
        gettimeofday(&t1,nullptr);
        gettimeofday(&t2,nullptr);
        while(t2.tv_usec-t1.tv_usec<10000){
            func(array,size[i]);
            round++;
            gettimeofday(&t2,nullptr);
        }
        cout<<size[i]<<","<<round<<","<<t2.tv_usec-t1.tv_usec<<","<<1.0*(t2.tv_usec-t1.tv_usec)/round<<endl;
    }
    return 0;
}