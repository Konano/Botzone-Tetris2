#include <cstdio>
#include <algorithm>
#include <cstring>
#include <queue>
#include <bitset>
#include <cmath>
#include <set>
#include <map>
#include <iostream>
#include <cstdlib>
#include <ctime>
 
using namespace std;
 
#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define travel(x) for(edge *p=fir[x]; p; p=p->n)
#define all(x) (x).begin(),(x).end()
#define pb push_back
#define fi first
#define se second
#define l(x) Left[x]
#define r(x) Right[x]
#define lowbit(x) (x&-x)
 
typedef long long ll;
typedef pair<int,int> Pii;
typedef pair<ll,int> Pli;
typedef pair<double,int> Pdi;
typedef long double ld;
typedef unsigned long long ull;

#define NerM 11
#define NerN 8
#define NerNum 10
#define OPNum 1
#define HPinit 30
#define MaxNerNum 20000



inline double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}
inline double rand2(){return rand01()*2-1;}



struct Neuron
{
	int ActType;
	double weight[NerM], b0[NerN], theta[NerN];
} Ner[MaxNerNum+9]; int NerCnt, v[19], hp[19];

inline void NeuronInit(int a)
{
	v[a]=++NerCnt, hp[a]=HPinit;
	rep(i, 0, NerM-1) Ner[v[a]].weight[i]=rand2();
	rep(i, 0, NerN-1) Ner[v[a]].b0[i]=rand2();
	rep(i, 0, NerN-1) Ner[v[a]].theta[i]=rand2();
	Ner[v[a]].ActType=rand()%8;
}

inline void NeuronVary(Neuron &a, double o)
{
	rep(i, 0, NerM-1) if (rand01()<o) a.weight[i]=rand2();
	rep(i, 0, NerN-1) if (rand01()<o) a.b0[i]=rand2();
	rep(i, 0, NerN-1) if (rand01()<o) a.theta[i]=rand2();
	if (rand01()<o) a.ActType=rand()%8;
}

inline void NeuronComb(Neuron &a, Neuron &b, Neuron &c, double o)
{
	rep(i, 0, NerM-1) c.weight[i]=(rand01()<o?a:b).weight[i];
	rep(i, 0, NerN-1) c.b0[i]=(rand01()<o?a:b).b0[i];
	rep(i, 0, NerN-1) c.theta[i]=(rand01()<o?a:b).theta[i];
	c.ActType=(rand01()<o?a:b).ActType;
}



inline void Death(int a)
{
	rep(i, a+1, NerNum) swap(v[i-1], v[i]), swap(hp[i-1], hp[i]);
	a=NerNum; v[a]=++NerCnt, hp[a]=HPinit;
	NeuronComb(Ner[v[rand()%4+1]], Ner[v[rand()%4+1]], Ner[v[a]], 0.5+rand2()*0.2);
	NeuronVary(Ner[v[a]], rand01()/10*3);
	
	while (NerCnt%100==v[1]%100)
	{
		rep(i, 2, NerNum) swap(v[i-1], v[i]), swap(hp[i-1], hp[i]);
		hp[NerNum]=min(hp[NerNum], HPinit*2);
	}
}



inline void OutputNer(int a) 
{
	rep(i, 0, NerM-1) printf("%.9lf%c", Ner[v[a]].weight[i], i==NerM-1?'\n':' ');
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[v[a]].b0[i], i==NerN-1?'\n':' ');
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[v[a]].theta[i], i==NerN-1?'\n':' ');
	printf("%d\n", Ner[v[a]].ActType);
	puts("");
}

inline void OutputData()
{
	char filename[16]="data\\train3.txt";
	
	freopen(filename, "w", stdout);
	rep(o, 1, NerNum) printf("%d %d\n", v[o], hp[o]), OutputNer(o);
	fclose(stdout);
}

inline void InputNer(int a) 
{
	scanf("%d%d", &v[a], &hp[a]);
	rep(i, 0, NerM-1) scanf("%lf", &Ner[v[a]].weight[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[v[a]].b0[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[v[a]].theta[i]);
	scanf("%d", &Ner[v[a]].ActType);
}

inline void InputData()
{
	char filename[16]="data\\train3.txt";
	
	freopen(filename, "r", stdin);
	rep(o, 1, NerNum) InputNer(o);
	fclose(stdin);
	
	NerCnt=v[NerNum];
}

inline int Game(int player0, int player1)
{
	freopen("player0.txt", "w", stdout);
	OutputNer(player0);
	fclose(stdout);
	
	freopen("player1.txt", "w", stdout);
	OutputNer(player1);
	fclose(stdout);
	
	system("offline.exe");
	
	int Result=0;
	freopen("result.txt", "r", stdin);
	scanf("%d", &Result);
	fclose(stdin);
	
	if (Result>0) hp[player0]+=2, hp[player1]-=3;
	if (!Result) hp[player0]--, hp[player1]--;
	if (Result<0) hp[player0]-=3, hp[player1]+=2;
	
	return Result;
}

inline void OutputSomething()
{
	puts("Alive:");
	rep(i, 1, NerNum) printf("%d%c", v[i], i==NerNum?'\n':'\t');
	rep(i, 1, NerNum) printf("%d%c", hp[i], i==NerNum?'\n':'\t');
	puts("");
}

int TIM=0;
int main()
{
	//srand(time(NULL));
	
	int tmp; scanf("%d", &tmp);
	if (tmp) InputData(); else rep(i, 1, NerNum) NeuronInit(i);
	
	int TIM=0; while (NerCnt<=MaxNerNum)
	{
		int A=0, B=0;
		while (A==B) A=rand()%NerNum+1, B=rand()%NerNum+1;
		Game(A,B); if (hp[A]<=0) Death(A); if (hp[B]<=0) Death(B);
		
		TIM++; if (TIM%10==0)
		{
			OutputData();
			
			freopen("con", "w", stdout);
			
			OutputSomething();
			
			freopen("data\\history.txt", "a", stdout);
			
			OutputSomething();
		}
	}
	
	return 0;
}

