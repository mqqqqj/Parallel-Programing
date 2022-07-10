#include<iostream>
#include<cmath>
#include<math.h>
#include<stdlib.h>
#include "mpi.h"
using namespace std;

//城市节点数量
#define N 30
//蚂蚁数量
#define M 30

//信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
double Alpha = 2, Beta = 2, rou = 0.05,  qzero = 0.85;
double Lnn;//最邻近算法求解的最优路径
double global_best_len = 666;//全局最优路径长度
int global_best_tour[N];//全局最优路径
int best_ant = -1;//一次搜索中表现最好的蚂蚁
//题目的参数


double City[N][2]={
{2,99},{4,50},{7,64},{13,40},{18,54},{18,40},{22,60},{24,42},{25,62},{25,38},
{37,84},{41,94},{41,26},{44,35},{45,21},{54,67},{54,62},{58,35},{58,69},{62,32},
{64,60},{68,58},{71,44},{71,71},{74,78},{82,7},{83,46},{83,69},{87,76},{91,38}
};






double Info[N][N],Visable[N][N],Distance[N][N];//信息素矩阵\能见度矩阵\城市距离矩阵

//如何使结果收敛，控制循环次数还是当结果在某一可信度范围内不变时就结束，暂且先给出每只蚂蚁的搜索次数
int T = 1000;

double caltwodistance(int i,int j) {//计算两个城市的距离
    return sqrt(pow((City[i][0] - City[j][0]), 2.0) + pow((City[i][1] - City[j][1]), 2.0));
}

void calalldistance() {//计算所有城市之间的距离，构造城市距离矩阵
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Distance[i][j] = caltwodistance(i, j);
            Distance[j][i] = Distance[i][j];
        }
    }
}

class ANT {
public:
    int step;
    int cururentCity;
    bool visited[N]; //禁忌表
    int tour[N]; //搜索路径
    double tour_length; //路径长度

    void callength(); //计算路径长度
    double Pchoose(int i ,int j);
    void PrivateupdateInfo();
    int ChooseNextCity();
};

void ANT::callength() {
    tour_length = 0;
    for(int i = 0;i < N;i ++){
        tour_length += Distance[tour[(i+N-1)%N]][tour[i]];
    }
}

double ANT::Pchoose(int i, int j) {
    double Numerator = 0, Denominator = 0;
    for(int k = 0;k < N;k ++){
        if(visited[k] == false){
            Denominator += pow(Info[i][k],Alpha)+pow(Visable[i][k],Beta);
        }
    }
    Numerator = pow(Info[i][j],Alpha)+pow(Visable[i][j],Beta);
    //cout<<Numerator/Denominator<<endl;
    return Numerator/Denominator;
}

void ANT::PrivateupdateInfo() {//蚂蚁每走一步进行一次更新
    int x= tour[step-1], y = tour[step];
    Info[x][y] = (1-rou)*Info[x][y]+1/(N*Lnn);
    Info[y][x] = Info[x][y];
}

int ANT::ChooseNextCity() {
    double q = ((rand()%255)*1.0)/255;
    int res = cururentCity;
    if(q<qzero){
        double prob = 0;
        for(int i = 0;i < N;i ++){
            if(visited[i]==false){
                double p = Pchoose(cururentCity,i);
                if(p>prob){
                    prob = p;
                    res = i;
                }
            }
        }
    }else{
        int avail[N],k = 0;
        for(int i = 0;i < N;i ++){
            if(visited[i] == false)
                avail[k++] = i;
        }
        int index = rand()%k;
        res = avail[index];
    }
    return res;
}

void GlobalUpdateInfo(int *path){
    for(int i = 0;i < N;i ++){
        int x = path[(i-1+N)%N],y = path[i];
        Info[x][y] = (1-rou)*Info[x][y] + rou*1.0/global_best_len;
    }
}


void InitInfoAndVisable(){//初始化信息素和能见度矩阵
    //给定一个初始化信息素矩阵的方法，任选一个起点，使用最邻近算法遍历所有城市，即每次都选取离当前城市最近的城市。

    int length = 0,curr = rand()%N;
    bool visited[N];
    for(int i = 0;i < N;i ++){
        visited[i]= false;
    }
    visited[curr] = true;
    for(int i = 0;i < N-1;i ++){//N-1次使用最邻近算法构建完成路径
        int nearest = -1,nearest_len = 999;
        for(int j = 0;j < N;j ++){ //找到离当前城市最近的城市
            if(visited[j]== false && j != curr){
                if(Distance[curr][j]<nearest_len){
                    nearest_len = Distance[curr][j];
                    nearest = j;
                }
            }
        }
        visited[nearest] = true;
        length += Distance[curr][nearest];
        //cout<<curr<<" -> "<<nearest<<": "<<Distance[curr][nearest]<<endl;
        curr = nearest;
    }
    for(int i = 0;i < N;i ++){
        for(int j = 0; j < N;j ++){
            Info[i][j] = 1.0/(N*length);
            Info[j][i] = 1.0/(N*length);
            if(i!=j){
                Visable[i][j] = 1.0/Distance[i][j];
                Visable[j][i] = Visable[i][j];
            }
        }
    }
    Lnn = length;
}

int main(int argc, char *argv[]){
    srand(time(0));
    calalldistance();//先计算出所有城市之间的距离
    InitInfoAndVisable(); //初始化信息素矩阵和能见度矩阵
    ANT ant[M];
    int myid,numprocs;
    double start, end;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Status status;
    int heavier = M%numprocs;
    int Nwork = M/numprocs;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    for(int t = 0;t < T;t ++){
        if(myid<heavier){
            for(int j = myid*(Nwork+1);j <(myid+1)*(Nwork+1);j ++){//初始化蚂蚁
                for(int n = 0;n < N;n ++){
                    ant[j].visited[n] = false;
                    ant[j].tour[n] = -1;
                }
                ant[j].tour_length = 0;
                ant[j].visited[j%N] = true;
                ant[j].cururentCity = j%N;
                ant[j].tour[0] = j%N;
                ant[j].step = 0;
                //蚂蚁开始搜索
                while(ant[j].step<N-1){
                    ant[j].step++;
                    int c = ant[j].ChooseNextCity();
                    ant[j].tour[ant[j].step] = c;
                    ant[j].visited[c] = true;
                    ant[j].cururentCity = c;
                    ant[j].PrivateupdateInfo();
                }
                ant[j].callength();
                //cout<<ant[j].tour_length<<endl;
                MPI_Send(&(ant[j].tour_length),1,MPI_DOUBLE,numprocs-1,99,MPI_COMM_WORLD);
            }
        }else {
            for(int j = heavier*(Nwork+1)+(myid-heavier)*Nwork;j < heavier*(Nwork+1)+(myid+1-heavier)*Nwork;j ++){
                for(int n = 0;n < N;n ++){
                    ant[j].visited[n] = false;
                    ant[j].tour[n] = -1;
                }
                ant[j].tour_length = 0;
                ant[j].visited[j%N] = true;
                ant[j].cururentCity = j%N;
                ant[j].tour[0] = j%N;
                ant[j].step = 0;
                //蚂蚁开始搜索
                while(ant[j].step<N-1){
                    ant[j].step++;
                    int c = ant[j].ChooseNextCity();
                    ant[j].tour[ant[j].step] = c;
                    ant[j].visited[c] = true;
                    ant[j].cururentCity = c;
                    ant[j].PrivateupdateInfo();
                }
                ant[j].callength();
                MPI_Send(&(ant[j].tour_length),1,MPI_DOUBLE,numprocs-1,j,MPI_COMM_WORLD);
            }
        }
        //所有蚂蚁完成了搜索,让最后一个进程来搜索局部最优路径
        if(myid==numprocs-1){
            for(int i = 0;i < M;i ++){
                MPI_Recv(&(ant[i].tour_length),1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            }
            double local_best_len = 999;
            for(int i = 0;i < M;i ++){
                if(ant[i].tour_length<local_best_len){
                    local_best_len = ant[i].tour_length;
                    best_ant = i;
                }
            }
            if(local_best_len<global_best_len){
                global_best_len = local_best_len;
                GlobalUpdateInfo(global_best_tour);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    cout<<end-start<<endl;
    
    MPI_Finalize();
    cout<<global_best_len<<endl;
    return 0;
}
