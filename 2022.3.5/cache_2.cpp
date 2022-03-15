#include<cstdlib>
#include<time.h> 
#include <sys/time.h>
#include <unistd.h>
#include<iostream>
#include<cstring>
using namespace std;
const int size[13]={2,4,8,16,32,64,128,256,512,1024,2048,4096,8192};
int res[8192],array[8192];
int matrix[8192][8192];
void func(const int N){
    struct timeval tv1,tv2;
    int round = 0;
    gettimeofday(&tv1,nullptr);
    gettimeofday(&tv2,nullptr);
    while((tv2.tv_usec-tv1.tv_usec)<10000){
        round ++;
        for(int i = 0;i < N;i ++){
            res[i] = 0;
        }
        for(int j = 0;j < N;j ++){
            for(int i = 0;i < N;i ++){
                res[i]+=matrix[j][i]*array[j];
            }
    }
    gettimeofday(&tv2,nullptr);
    }
    cout<<N<<","<<round<<","<<tv2.tv_usec-tv1.tv_usec<<","<<1.0*(tv2.tv_usec-tv1.tv_usec)/round<<endl;
}

int main(){
    srand(time(NULL));
    memset(array,rand()%8192,sizeof(array));
    for(int i = 0;i < 8192;i ++){
        for(int j = 0;j < 8192;j ++){
            matrix[i][j]=i+j;
        }
    }
    for(int i = 0;i < 13;i ++){
        func(size[i]);
    }
    return 0;
}
