#include<stdlib.h>   
#include<stdio.h>
#include<math.h>   
#include <cmath>
#include<time.h>   

using namespace std;
//全局变量
#define N 30
#define M 30
float C[N][2]={
{2,99},{4,50},{7,64},{13,40},{18,54},{18,40},{22,60},{24,42},{25,62},{25,38},
{37,84},{41,94},{41,26},{44,35},{45,21},{54,67},{54,62},{58,35},{58,69},{62,32},
{64,60},{68,58},{71,44},{71,71},{74,78},{82,7},{83,46},{83,69},{87,76},{91,38}
};

int NcMax = 500;
float alpha = 2, beta = 3, rou = 0.1, alpha1 = 0.1,  qzero = 0.01;
float Lnn;//最近邻方法得到的一个长度  
float allDistance[N][N], info[N][N], visible[N][N];//两两之间的距离,信息素强度,能见度   

float calculateDistance(int i, int j){
	return sqrt(pow((C[i][0] - C[j][0]), 2.0) + pow((C[i][1] - C[j][1]), 2.0));
}

void calculateAllDistance(){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (i != j&&allDistance[i][j]==0){
				allDistance[i][j] = calculateDistance(i, j);
				allDistance[j][i] = allDistance[i][j];
			}
		}
	}
}

float calculateSumOfDistance(int* tour){
	float sum = 0;
	for (int i = 0; i< N; i++){
		int row = *(tour + 2 * i);
		int col = *(tour + 2 * i + 1);
		sum += allDistance[row][col];
	}
	return sum;
}

float Transition(int i, int j){
	if (i != j){
		return (pow(info[i][j], alpha) * pow(visible[i][j], beta));
	}else{
		return 0.0;
	}
}
 
void UpdateLocalPathRule(int i, int j){//局部更新规则  
	info[i][j] = (1.0 - alpha1) * info[i][j] + alpha1 * (1.0 / (N*Lnn));
	info[j][i] = info[i][j];
}
  
void UpdateGlobalPathRule(int* bestTour, int globalBestLength){//全局信息素更新 
	for (int i = 0; i < N; i++){
		int row = *(bestTour + 2 * i);
		int col = *(bestTour + 2 * i + 1);
		info[row][col] = (1.0 - rou) * info[row][col] + rou * (1.0 / globalBestLength);
		info[col][row] = info[row][col];
	}
}
typedef struct{
	int startCity, currentCity;//初始城市编号，当前城市编号   
	int allowed[N];//禁忌表       
	int Tour[N][2];//当前路径   
	int currentTourIndex;//当前路径索引，从0开始，存储蚂蚁经过城市的编号  
}ACSAnt;

//选择下一节点   
int Choose(ACSAnt ant){
	int nextCity = -1;
	float q = rand() / (float)RAND_MAX;
	//如果 q <= q0,按先验知识，否则则按概率转移，   
	if (q <= qzero){
		float probability = -1.0;//转移到下一节点的概率   
		for (int i = 0; i < N; i++){
			//去掉禁忌表中已走过的节点,从剩下节点中选择最大概率的可行节点   
			if (1 == ant.allowed[i]){
				float prob = Transition(ant.currentCity, i);
				if (prob  > probability){
					nextCity = i;
					probability = prob;
				}
			}
		}
	}
	else{
		//按概率转移            
		float p = rand() / (float)RAND_MAX;//生成一个随机数,用来判断落在哪个区间段   
		float sum = 0.0;
		float probability = 0.0;//概率的区间点，p 落在哪个区间段，则该点是转移的方向   
		//计算概率公式的分母的值   
		for (int i = 0; i < N; i++){
			if (1 == ant.allowed[i]){
				sum += Transition(ant.currentCity, i);
			}
		}
		for (int j = 0; j < N; j++){
			if (1 == ant.allowed[j] && sum > 0){
				probability += Transition(ant.currentCity, j) / sum;
				if (probability >= p || (p > 0.9999 && probability > 0.9999)){
					nextCity = j;
					break;
				}
			}
		}
	}
	return nextCity;
}

//移动到下一节点   
void MoveToNextCity(ACSAnt ant,int nextCity){
	ant.allowed[nextCity] = 0;
	ant.Tour[ant.currentTourIndex][0] = ant.currentCity;
	ant.Tour[ant.currentTourIndex][1] = nextCity;
	ant.currentTourIndex++;
	ant.currentCity = nextCity;
}

//开始搜索   
void* Search(ACSAnt ant){
	ant.currentCity = ant.startCity;
	int toCity;
	ant.currentTourIndex = 0;
	for (int i = 0; i < N; i++){
		ant.allowed[i] = 1;
	}
	ant.allowed[ant.currentCity] = 0;
	do{
		toCity = Choose(ant);
		if (toCity >= 0){
			MoveToNextCity(ant,toCity);
            UpdateLocalPathRule(ant.currentCity, toCity);
			ant.currentCity = toCity;
		}
	} while (toCity >= 0);
	MoveToNextCity(ant,ant.startCity);
    UpdateLocalPathRule(ant.currentCity, ant.startCity);
}

//------------------------------------------   
//给一个节点由最近邻距离方法计算长度 ,使用下面两个函数，只计算一次  
int ChooseNextNode(int currentNode, int visitedNode[]){
	int nextNode = -1;
	float shortDistance = 0.0;
	for (int i = 0; i < N; i++){
		//去掉已走过的节点,从剩下节点中选择距离最近的节点   
		if (1 == visitedNode[i]){
			if (shortDistance == 0.0){
				shortDistance = allDistance[currentNode][i];
				nextNode = i;
			}
			if (shortDistance < allDistance[currentNode][i]){
				nextNode = i;
			}
		}
	}
	return nextNode;
}
  
float CalAdjacentDistance(int node){
	float sum = 0.0;
	int visitedNode[N];
	for (int j = 0; j < N; j++){
		visitedNode[j] = 1;
	}
	visitedNode[node] = 0;
	int currentNode = node;
	int nextNode;
	do{
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
 
int main(){
    struct timespec sts,ets;
    timespec_get(&sts, TIME_UTC); 
	ACSAnt ants[M];//一个线程就是一只蚂蚁
	for (int k = 0; k < M; k++)//蚂蚁均匀分布在城市上
		ants[k].startCity=(int)(k%N);
	calculateAllDistance();
	//随机选择一个节点计算由最近邻方法得到的一个长度   
	Lnn = CalAdjacentDistance(rand() % N);
	float initInfo = 1 / ( N*Lnn);//初始化各条路径上的信息素强度 
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			info[i][j] = initInfo;
			info[j][i] = initInfo;
			if (i != j){
				visible[i][j] = 1.0 / allDistance[i][j];
				visible[j][i] = visible[i][j];
			}
		}
	}
	//全局最优路径   
	int globalTour[N][2];
	//全局最优长度   
	float globalBestLength = 0.0;

	for (int i = 0; i < NcMax; i++){//进行数百次循环，在每一次循环中，每一个线程都要进行一次完整的操作，因此需要在外部循环结束一次后，重新初始化线程的参数
		int localTour[N][2];//局部最优路径 ，这里的局部指的是每一次外部循环结束后确定的局部最优路径  
		float localBestLength = 0.0;//局部最优长度   
		float tourLength = 0.0;//当前路径长度   
		for (int j = 0; j < M; j++){
			Search(ants[j]);
			int* tourPath = ants[j].*Tour;
			tourLength = calculateSumOfDistance(tourPath);
			//局部比较，并记录路径和长度   
			if (tourLength < localBestLength || fabs(localBestLength - 0.0) < 0.000001){
				for (int m = 0; m< N; m++){
					int row = *(tourPath + 2 * m);
					int col = *(tourPath + 2 * m + 1);
					localTour[m][0] = row;
					localTour[m][1] = col;
				}
				localBestLength = tourLength;
			}
		}
		//全局比较，并记录路径和长度   
		if (localBestLength < globalBestLength || fabs(globalBestLength - 0.0) < 0.000001){
			for (int m = 0; m< N; m++)
			{
				globalTour[m][0] = localTour[m][0];
				globalTour[m][1] = localTour[m][1];
			}
			globalBestLength = localBestLength;
		}
		UpdateGlobalPathRule(*globalTour, globalBestLength);
	}

	//计算时间
    timespec_get(&ets, TIME_UTC);
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
