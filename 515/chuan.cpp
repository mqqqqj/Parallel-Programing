#include<iostream>
#include <cmath>
#include<time.h>
using namespace std;

//该程序是以蚁群系统为模型写的蚁群算法程序(强调：非蚂蚁周模型)，以三个著名的TSP问题为测试对象
//通过微调参数，都可以获得较好的解

//----------(1)问题一：Oliver 30 城市 TSP 问题 best_length = 423.7406; ------------------------
//该程序最好的结果是423.741，可运行多次获得
//城市节点数目
#define N 30
//城市坐标
double C[N][2]={
{2,99},{4,50},{7,64},{13,40},{18,54},{18,40},{22,60},{24,42},{25,62},{25,38},
{37,84},{41,94},{41,26},{44,35},{45,21},{54,67},{54,62},{58,35},{58,69},{62,32},
{64,60},{68,58},{71,44},{71,71},{74,78},{82,7},{83,46},{83,69},{87,76},{91,38}
};
//----------上面参数是固定的，下面的参数是可变的-----------
//蚂蚁数量
#define M 30
//最大循环次数NcMax
int NcMax = 500;
//信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
double alpha = 2, beta = 3, rou = 0.1, alpha1 = 0.1,  qzero = 0.01;
//-----------问题一结束------------------------------------------------------------------------


/*
//----------(2)问题二：Elion50 城市 TSP 问题 best_length = 427.96; ----------------------------
//该程序最好的结果是428.468，可运行多次获得
//城市节点数目
#define N 50
//城市坐标
double C[N][2]={
{5,64}, {5,25}, {5,6}, {7,38}, {8,52}, {10,17},
{12,42}, {13,13}, {16,57}, {17,33}, {17,63},
{20,26}, {21,47}, {21,10}, {25,32}, {25,55},
{27,68}, {27,23}, {30,48}, {30,15}, {31,62},
{31,32}, {32,22}, {32,39}, {36,16}, {37,69},
{37,52}, {38,46}, {39,10}, {40,30}, {42,57},
{42,41}, {43,67}, {45,35}, {46,10}, {48,28},
{49,49}, {51,21}, {52,33}, {52,41}, {52,64},
{56,37}, {57,58}, {58,27}, {58,48}, {59,15},
{61,33}, {62,42}, {62,63}, {63,69}
};
//----------上面参数是固定的，下面的参数是可变的-----------
//蚂蚁数量
#define M 50
//最大循环次数NcMax
int NcMax = 1000;
//信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
double alpha = 2, beta = 4, rou = 0.1, alpha1 = 0.1,  qzero = 0.01;
//-----------问题二结束------------------------------------------------------------------------




//----------(3)问题三：Elion75 城市 TSP 问题 best_length = 542.31;
//该程序最好的结果是542.309，可运行多次获得
//城市节点数目
#define N 75
//城市坐标
double C[N][2] = {
        { 6, 25 }, { 7, 43 }, { 9, 56 }, { 10, 70 }, { 11, 28 },
        { 12, 17 }, { 12, 38 }, { 15, 5 }, { 15, 14 }, { 15, 56 },
        { 16, 19 }, { 17, 64 }, { 20, 30 }, { 21, 48 }, { 21, 45 },
        { 21, 36 }, { 22, 53 }, { 22, 22 }, { 26, 29 }, { 26, 13 },
        { 26, 59 }, { 27, 24 }, { 29, 39 }, { 30, 50 }, { 30, 20 },
        { 30, 60 }, { 31, 76 }, { 33, 34 }, { 33, 44 }, { 35, 51 },
        { 35, 16 }, { 35, 60 }, { 36, 6 }, { 36, 26 }, { 38, 33 },
        { 40, 37 }, { 40, 66 }, { 40, 60 }, { 40, 20 }, { 41, 46 },
        { 43, 26 }, { 44, 13 }, { 45, 42 }, { 45, 35 }, { 47, 66 },
        { 48, 21 }, { 50, 30 }, { 50, 40 }, { 50, 50 }, { 50, 70 },
        { 50, 4 }, { 50, 15 }, { 51, 42 }, { 52, 26 }, { 54, 38 },
        { 54, 10 }, { 55, 34 }, { 55, 45 }, { 55, 50 }, { 55, 65 },
        { 55, 57 }, { 55, 20 }, { 57, 72 }, { 59, 5 }, { 60, 15 },
        { 62, 57 }, { 62, 48 }, { 62, 35 }, { 62, 24 }, { 64, 4 },
        { 65, 27 }, { 66, 14 }, { 66, 8 }, { 67, 41 }, { 70, 64 }
};
//----------上面参数是固定的，下面的参数是可变的-----------
//蚂蚁数量
#define M 75
//最大循环次数NcMax
int NcMax = 1000;
//信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
double alpha = 2, beta = 5, rou = 0.1, alpha1 = 0.1, qzero = 0.1;
//-----------问题三结束------------------------------------------------------------------------
*/

//===========================================================================================================
//局部更新时候使用的的常量，它是由最近邻方法得到的一个长度
//什么是最近邻方法?:)就是从源节点出发，每次选择一个距离最短的点来遍历所有的节点得到的路径
//每个节点都可能作为源节点来遍历
double Lnn;
//矩阵表示两两城市之间的距离
double allDistance[N][N];

//计算两个城市之间的距离
double calculateDistance(int i, int j)
{
    return sqrt(pow((C[i][0] - C[j][0]), 2.0) + pow((C[i][1] - C[j][1]), 2.0));
}

//由矩阵表示两两城市之间的距离
void calculateAllDistance()//这里可以并行化
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i != j)
            {
                allDistance[i][j] = calculateDistance(i, j);
                allDistance[j][i] = allDistance[i][j];
            }
        }
    }
}

//获得经过n个城市的路径长度
double calculateSumOfDistance(int* tour)      //并行化
{
    double sum = 0;
    for (int i = 0; i< N; i++)
    {
        int row = *(tour + 2 * i);
        int col = *(tour + 2 * i + 1);
        sum += allDistance[row][col];
    }
    return sum;
}

class ACSAnt;

class AntColonySystem
{
private:
    double info[N][N], visible[N][N];//节点之间的信息素强度,节点之间的能见度
public:
    AntColonySystem(){}
    //计算当前节点到下一节点转移的概率
    double Transition(int i, int j);
    //局部更新规则
    void UpdateLocalPathRule(int i, int j);
    //初始化
    void InitParameter(double value);
    //全局信息素更新
    void UpdateGlobalPathRule(int* bestTour, int globalBestLength);
};

//计算当前节点到下一节点转移的概率
double AntColonySystem::Transition(int i, int j)
{
    if (i != j)
    {
        return (pow(info[i][j], alpha) * pow(visible[i][j], beta));
    }
    else
    {
        return 0.0;
    }
}
//局部更新规则，更新两个城市之间的信息素浓度
void AntColonySystem::UpdateLocalPathRule(int i, int j)
{
    info[i][j] = (1.0 - alpha1) * info[i][j] + alpha1 * (1.0 / (N*Lnn));
    info[j][i] = info[i][j];
}
//初始化，初始化全局所有城市之间的信息素浓度和可见度。
void AntColonySystem::InitParameter(double value)//并行化
{
    //初始化路径上的信息素强度tao0
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            info[i][j] = value;
            info[j][i] = value;
            if (i != j)
            {
                visible[i][j] = 1.0 / allDistance[i][j];
                visible[j][i] = visible[i][j];
            }
        }
    }
}

//全局信息素更新，用最好的路径来更新
void AntColonySystem::UpdateGlobalPathRule(int* bestTour, int globalBestLength)//并行化
{
    for (int i = 0; i < N; i++)
    {
        int row = *(bestTour + 2 * i);
        int col = *(bestTour + 2 * i + 1);
        info[row][col] = (1.0 - rou) * info[row][col] + rou * (1.0 / globalBestLength);
        info[col][row] = info[row][col];
    }
}

class ACSAnt//蚂蚁类
{
private:
    AntColonySystem* antColony;
protected:
    int startCity, cururentCity;//初始城市编号，当前城市编号
    int allowed[N];//禁忌表
    int Tour[N][2];//当前路径
    int currentTourIndex;//当前路径索引，从0开始，存储蚂蚁经过城市的编号
public:
    ACSAnt(AntColonySystem* acs, int start){
        antColony = acs;
        startCity = start;
    }
    int* Search();//开始搜索
    int Choose();//选择下一节点
    void MoveToNextCity(int nextCity);//移动到下一节点
};

int* ACSAnt::Search()//开始搜索
{
    cururentCity = startCity;
    int toCity;
    currentTourIndex = 0;
    for (int i = 0; i < N; i++)//初始化禁忌表，可用simd并行
    {
        allowed[i] = 1;
    }
    allowed[cururentCity] = 0;
    int endCity;
    int count = 0;
    do//每次搜索都把所有城市都遍历，找到最好的
    {
        count++;
        endCity = cururentCity;
        toCity = Choose();
        if (toCity >= 0)
        {
            MoveToNextCity(toCity);
            cururentCity = toCity;
            antColony->UpdateLocalPathRule(endCity, toCity);//这里是临界
        }
    } while (toCity >= 0);
    MoveToNextCity(startCity);
    antColony->UpdateLocalPathRule(endCity, startCity);//这里是临界
    return *Tour;
}

int ACSAnt::Choose()//选择下一节点
{
    int nextCity = -1;
    double q = rand() / (double)100;
    //如果 q <= q0,按先验知识，否则则按概率转移，
    if (q <= qzero*100)
    {
        double probability = -1.0;//转移到下一节点的概率
        for (int i = 0; i < N; i++)
        {
            //去掉禁忌表中已走过的节点,从剩下节点中选择最大概率的可行节点
            if (1 == allowed[i])
            {
                double prob = antColony->Transition(cururentCity, i);//计算转移概率
                if (prob  > probability)
                {
                    nextCity = i;
                    probability = prob;
                }
            }
        }
    }
    else //按概率转移
    {
        double p = rand() / (double)RAND_MAX;//生成一个随机数,用来判断落在哪个区间段
        double sum = 0.0;
        double probability = 0.0;//概率的区间点，p 落在哪个区间段，则该点是转移的方向
        //计算概率公式的分母的值
        for (int i = 0; i < N; i++)
        {
            if (1 == allowed[i])
            {
                sum += antColony->Transition(cururentCity, i);
            }
        }

        for (int j = 0; j < N; j++)
        {
            if (1 == allowed[j] && sum > 0)
            {
                probability += antColony->Transition(cururentCity, j) / sum;
                if (probability >= p || (p > 0.9999 && probability > 0.9999))
                {
                    nextCity = j;
                    break;
                }
            }
        }
    }
    return nextCity;
}

void ACSAnt::MoveToNextCity(int nextCity)//移动到下一节点
{
    allowed[nextCity] = 0;
    Tour[currentTourIndex][0] = cururentCity;
    Tour[currentTourIndex][1] = nextCity;
    currentTourIndex++;
    cururentCity = nextCity;
}

//------------------------------------------
//选择下一个节点，配合下面的函数来计算的长度
int ChooseNextNode(int currentNode, int visitedNode[])
{
    int nextNode = -1;
    double shortDistance = 0.0;
    for (int i = 0; i < N; i++)
    {
        //去掉已走过的节点,从剩下节点中选择距离最近的节点
        if (1 == visitedNode[i])
        {
            if (shortDistance == 0.0)
            {
                shortDistance = allDistance[currentNode][i];
                nextNode = i;
            }
            if (shortDistance < allDistance[currentNode][i])
            {
                nextNode = i;
            }
        }
    }
    return nextNode;
}

//给一个节点由最近邻距离方法计算长度
double CalAdjacentDistance(int node)
{
    double sum = 0.0;
    int visitedNode[N];
    for (int j = 0; j < N; j++)
    {
        visitedNode[j] = 1;
    }
    visitedNode[node] = 0;
    int currentNode = node;
    int nextNode;
    do
    {
        nextNode = ChooseNextNode(currentNode, visitedNode);
        if (nextNode >= 0)
        {
            sum += allDistance[currentNode][nextNode];
            currentNode = nextNode;
            visitedNode[currentNode] = 0;
        }
    } while (nextNode >= 0);
    sum += allDistance[currentNode][node];
    return sum;
}

int main()
{
    srand(time(0));
    calculateAllDistance();//计算所有城市之间的距离
    AntColonySystem* acs = new AntColonySystem();//蚁群系统对象
    ACSAnt* ants[M];//蚁群对象指针数组
    //蚂蚁均匀分布在城市上
    for (int k = 0; k < M; k++)
    {
        ants[k] = new ACSAnt(acs, (int)(k%N));
    }
    calculateAllDistance();
    int node = rand() % N;//随机选择一个节点计算由最近邻方法得到的一个长度
    Lnn = CalAdjacentDistance(node);
    double initInfo = 1 / ( N*Lnn);//各条路径上初始化的信息素强度，依赖前面的随机取值
    acs->InitParameter(initInfo);
    int globalTour[N][2];//定义全局最优路径
    double globalBestLength = 0.0;//全局最优长度
    struct timespec sts,ets;
    timespec_get(&sts, TIME_UTC); 
    //开始核心计算
    for (int i = 0; i < NcMax; i++)//所有蚂蚁进行一千次模拟
    {
        //局部最优路径
        int localTour[N][2];
        //局部最优长度
        double localBestLength = 0.0;
        //当前路径长度
        double tourLength;
        for (int j = 0; j < M; j++)//每个蚂蚁开始行动
        {
            int* tourPath = ants[j]->Search();
            tourLength = calculateSumOfDistance(tourPath);
            //局部比较，并记录路径和长度
            if (tourLength < localBestLength || fabs(localBestLength - 0.0) < 0.000001)
            {
                for (int m = 0; m< N; m++)
                {
                    int row = *(tourPath + 2 * m);
                    int col = *(tourPath + 2 * m + 1);
                    localTour[m][0] = row;//临界
                    localTour[m][1] = col;//临界
                }
                localBestLength = tourLength;//临界
            }
        }
        //全局比较，并记录路径和长度
        if (localBestLength < globalBestLength || fabs(globalBestLength - 0.0) < 0.000001)
        {
            for (int m = 0; m< N; m++)
            {
                globalTour[m][0] = localTour[m][0];//临界
                globalTour[m][1] = localTour[m][1];//临界
            }
            globalBestLength = localBestLength;//临界
        }
        acs->UpdateGlobalPathRule(*globalTour, globalBestLength);//临界
        //输出所有蚂蚁循环一次后的迭代最优路径
        // cout << "the " << i + 1 << "  time best road:" << localBestLength << "." << endl;
        // for (int m = 0; m< N; m++)
        // {
        // 	cout << localTour[m][0] << ".";
        // }
        // cout << endl;
    }
    timespec_get(&ets, TIME_UTC);
    //输出全局最优路径
    cout << "best of the best:" << globalBestLength << endl;
    cout << "road :";
    for (int m = 0; m< N; m++)
    {
        cout << globalTour[m][0] << ".";
    }
    
    time_t dsec=ets.tv_sec-sts.tv_sec;
    long dnsec=ets.tv_nsec-sts.tv_nsec;
    if (dnsec<0){
        dsec--;
        dnsec+=1000000000ll;
    }
    printf("globalBestLength%f\n",globalBestLength);
    printf("%ld.%09lds\n",dsec,dnsec);
	return 0;
}
