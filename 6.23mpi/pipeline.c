#include <mpi.h>
#include <stdio.h>
#include<time.h>
#include <stdlib.h>
float A[8][2000];

void initial(int N,int M){
    srand((int)time(NULL));
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < M;j ++){
            A[i][j] = i+j+rand() % 100;
        }
    }
}

void gaosiSerial(int N,int M){
    for(int k = 0;k < N;k ++){
        for(int j = k + 1;j < M;j ++){
            A[k][j] = A[k][j]/A[k][k];
        }
        A[k][k] = 1.0;
        for(int i = k + 1;i < N;i ++){
            for(int j = k + 1;j < M;j ++){
                A[i][j] -= A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
    }
}

void showres(int N,int M){
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < M;j ++){
            printf(" %f",A[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int N = 8;
    int M = 2000;
    initial(N,M);
    int myid, namelen,numprocs;
    char message[20];

    struct timespec sts,ets;
    timespec_get(&sts, TIME_UTC);
    MPI_Status status;
    MPI_Request handle;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Get_processor_name(processor_name,&namelen);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    //流水线
    for(int k = 0;k < N;k++){
        if(myid==k){
            for(int j = k + 1;j < M;j ++){
                A[k][j] = A[k][j]/A[k][k];
            }
            A[k][k] = 1;
            if(myid!=N-1)
                MPI_Isend(&A[k][0],M,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,handle);
        }else if(myid>k&&myid<numprocs-1){
            MPI_Recv(&A[k][0],M,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD,&status);
            MPI_Isend(&A[k][0],M,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,handle);
        }else if(myid>k&&myid==numprocs-1){
            MPI_Recv(&A[k][0],M,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD,&status);
        }
        for(int i = 0;i < M;i ++){
            A[myid][i]-=A[k][i]*A[myid][k];
        }
        if(myid!=N-1)
            A[myid][k]=0;
    }
    MPI_Finalize();
    timespec_get(&ets, TIME_UTC);
    time_t dsec=ets.tv_sec-sts.tv_sec;
    long dnsec=ets.tv_nsec-sts.tv_nsec;
    if (dnsec<0){
        dsec--;
        dnsec+=1000000000ll;
    }
    printf ("%ld.%09lds\n",dsec,dnsec);
    return 0;
}
