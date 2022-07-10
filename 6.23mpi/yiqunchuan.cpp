#include<iostream>
#include<cmath>
#include<stdlib.h>
#include<time.h>
using namespace std;

//城市节点数量
#define N 30
//蚂蚁数量
#define M 30

//信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
double Alpha = 2, Beta = 2, rou = 0.05,  qzero = 0.85;
//题目的参数
double City[N][2]={
{2,99},{4,50},{7,64},{13,40},{18,54},{18,40},{22,60},{24,42},{25,62},{25,38},
{37,84},{41,94},{41,26},{44,35},{45,21},{54,67},{54,62},{58,35},{58,69},{62,32},
{64,60},{68,58},{71,44},{71,71},{74,78},{82,7},{83,46},{83,69},{87,76},{91,38}
};


double Info[N][N],Visable[N][N],Distance[N][N];//信息素矩阵\能见度矩阵\城市距离矩阵

//如何使结果收敛，控制循环次数还是当结果在某一可信度范围内不变时就结束，暂且先给出每只蚂蚁的搜索次数
int T = 500;

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
    int cururentCity;
    bool visited[N]; //禁忌表
    int tour[N]; //搜索路径
    double tour_length; //路径长度

    void callength(); //计算路径长度
    double Pchoose(int i ,int j);
    void updateInfo();
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

void ANT::updateInfo() {//蚂蚁进行一次完整的路径构造之后进行信息素矩阵的更新，所有信息素挥发，然后蚂蚁走过的路径的信息素有一定的增强。
    for(int i = 0;i < N;i ++){
        for(int j = 0;j < N;j ++){
            Info[i][j] = (1-rou)*Info[i][j] ;
        }
    }
    for(int i = 0;i < N;i ++){
        int x= tour[(i+N-1)%N], y = tour[i];
        Info[x][y] += 0.5/(tour_length);
        Info[y][x] = Info[x][y];
    }
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
    //cout<<length;
}

int main(){
    srand(time(0));
    struct timespec sts,ets;
    calalldistance();//先计算出所有城市之间的距离
    //double global_best_tour[N];//全局最优路径
    double global_best_len = 999;
    InitInfoAndVisable(); //初始化信息素矩阵和能见度矩阵
    ANT ant[M];
    timespec_get(&sts, TIME_UTC);
    for(int t = 0;t < T;t ++){
        //构造解决方案
        double local_best_len = 999;
        for(int k = 0;k < M;k ++){
            for(int i = 0;i < N;i ++){
                ant[k].visited[i] = false;
                ant[k].tour[i] = -1;
            }
        }
        int step = 0;
        for(int k = 0;k < M;k ++){
            ant[k].cururentCity = k;
            ant[k].visited[k] = true;
            ant[k].tour[0] = k;
        }
        while(step<N-1){
            step++;
            for(int k = 0;k < M;k ++){
                int c = ant[k].ChooseNextCity();
                ant[k].tour[step] = c;
                ant[k].visited[c] = true;
                ant[k].cururentCity = c;
            }
        }
        for(int k = 0;k < M;k ++){
            ant[k].callength();
            ant[k].updateInfo();
        }
        for(int k = 0;k < M;k ++){
            if(ant[k].tour_length<local_best_len)
                local_best_len = ant[k].tour_length;
            //cout<<local_best_len<<" ";
        }
        //cout<<endl;
        if(local_best_len<global_best_len)
            global_best_len = local_best_len;
    }
    timespec_get(&ets, TIME_UTC);
    time_t dsec=ets.tv_sec-sts.tv_sec;
    long dnsec=ets.tv_nsec-sts.tv_nsec;
	if (dnsec<0){
	    dsec--;
	    dnsec+=1000000000ll;
	}
	printf ("%ld.%09ld\n",dsec,dnsec);  
    cout<<global_best_len;
    return 0;
}
