#include <mpi.h>
#include <stdio.h>
#include<time.h>
#include <stdlib.h>
#include <immintrin.h>
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
    int N = size[0];
    long long head, tail , freq ; // timers
    initial(N);
    int myid, namelen,numprocs,r1,r2;
    char message[20];
    MPI_Status status;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Get_processor_name(processor_name,&namelen);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    if(myid<N%numprocs){
        r1 = myid*(N/numprocs)+myid;
        r2 = (myid+1)*(N/numprocs)+myid;
    }else if(myid<numprocs){
        r1 = (N/numprocs+1)*(N%numprocs)+(N/numprocs)*(myid-N%numprocs);
        r2 = (N/numprocs+1)*(N%numprocs)+(N/numprocs)*(myid+1-N%numprocs)-1;
    }

    struct timespec sts,ets;
    timespec_get(&sts, TIME_UTC);

    for(int k = 0;k < N;k++){
        if(k>=r1&&k<=r2){
        __m256 vt = _mm256_set1_ps(A[k][k]);
        int j = k + 1;
        for (; j + 8 <= N; j += 8) {
            __m256 va = _mm256_loadu_ps(&A[k][j]);
            va = _mm256_div_ps(va, vt);
            _mm256_storeu_ps(&A[k][j], va);
        }
        for (; j < N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for(int j = 0; j < numprocs;j ++){
            if(myid!=j)
                MPI_Send(&A[k][0],N,MPI_FLOAT,j,j,MPI_COMM_WORLD);
        }
        }else{
            MPI_Recv(&A[k][0],N,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        }
        for(int i = r1;i <= r2;i ++){
            __m256 vaik = _mm256_set1_ps(A[i][k]);
            int t = k + 1;
            for (; t + 8 <= N; t += 8) {
                __m256 vakj = _mm256_loadu_ps(&A[k][t]);
                __m256 vaij = _mm256_loadu_ps(&A[i][t]);
                __m256 vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_storeu_ps(&A[i][t], vaij);
            }
            for (; t < N; t++) {
                A[i][t] -= A[i][k] * A[k][t];
            }
            A[i][k] = 0.0;
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
    MPI_Finalize();
    return 0;
}
