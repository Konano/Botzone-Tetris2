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

#define NerM 433
#define NerN 11
#define NerNum 10
#define OPNum 1

#define Train1 4
#define Train2 1
#define Train3 2
#define Train4 3
#define MaxTIM 9999



inline double rand01(){return (1.0*rand()/RAND_MAX)+(1.0*rand()/RAND_MAX/RAND_MAX)+(1.0*rand()/RAND_MAX/RAND_MAX/RAND_MAX);}
inline double rand2(){return rand01()*2-1;}



struct Neuron
{
	int ActType, Label;
	double weight[NerM], b0[NerN], theta[NerN], b1;
} Ner[NerNum+OPNum]; int NerAllNum;

inline void NeuronInit(Neuron &a)
{
	rep(i, 0, NerM-1) a.weight[i]=rand2();
	rep(i, 0, NerN-1) a.b0[i]=rand2(), a.theta[i]=rand2();
	a.b1=rand2();
	a.ActType=rand()%8;
	a.Label=++NerAllNum;
}

inline void NeuronVary(Neuron &a, double o)
{
	if (rand()%10<(int)o) a.ActType=rand()%8;
	if (rand()%10<(int)o) a.b1=rand2();
	while (rand()%(int)(o*NerN/10+1)) a.b0[rand()%NerN]=rand2();
	while (rand()%(int)(o*NerN/10+1)) a.theta[rand()%NerN]=rand2();
	while (rand()%(int)(o*NerM/10+1)) a.weight[rand()%NerM]=rand2();
	a.Label=++NerAllNum;
}

inline void NeuronComb(Neuron &a, Neuron &b, Neuron &c, double o)
{
	int cnt=0;
	c.weight[cnt]=(rand01()>o?a:b).weight[cnt], cnt++;
	for(int i=1,rd=rand01()>o; i<=19; i++) c.weight[cnt]=(rd?a:b).weight[cnt], cnt++;
	rep(i, 1, 4) c.weight[cnt]=(rand01()>o?a:b).weight[cnt], cnt++;
	for(int i=1,rd=rand01()>o; i<=3; i++) c.weight[cnt]=(rd?a:b).weight[cnt], cnt++;
	rep(i, 1, 7) for(int j=1,rd=rand01()>o; j<=58; j++) c.weight[cnt]=(rd?a:b).weight[cnt], cnt++;
	
	rep(i, 0, NerN-1) c.b0[i]=(rand01()>o?a:b).b0[i];
	rep(i, 0, NerN-1) c.theta[i]=(rand01()>o?a:b).theta[i];
	c.b1=(rand01()>o?a:b).b1;
	c.ActType=(rand01()>o?a:b).ActType;
	c.Label=++NerAllNum;
}



int TIM=0;

inline void OutputNer(int a) 
{
	rep(i, 0, NerM-1) printf("%.9lf%c", Ner[a].weight[i], i==NerM-1?'\n':' ');
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[a].b0[i], i==NerN-1?'\n':' ');
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[a].theta[i], i==NerN-1?'\n':' ');
	printf("%.9lf\n", Ner[a].b1);
	printf("%d\n", Ner[a].ActType);
}

inline void OutputData()
{
	char filename[14]="data\\0000.txt";
	
	filename[5]='0'+TIM/1000;
	filename[6]='0'+TIM/100%10;
	filename[7]='0'+TIM/10%10;
	filename[8]='0'+TIM%10;
	freopen(filename, "w", stdout);
	
	rep(o, 0, NerNum-1) OutputNer(o), puts("");
	
	fclose(stdout);
}

inline void InputNer(int a) 
{
	rep(i, 0, NerM-1) scanf("%lf", &Ner[a].weight[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[a].b0[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[a].theta[i]);
	scanf("%lf", &Ner[a].b1);
	scanf("%d", &Ner[a].ActType);
	Ner[a].Label=++NerAllNum;
}

inline void InputData()
{
	char filename[14]="data\\0000.txt";
	
	filename[5]='0'+TIM/1000;
	filename[6]='0'+TIM/100%10;
	filename[7]='0'+TIM/10%10;
	filename[8]='0'+TIM%10;
	freopen(filename, "r", stdin);
	
	rep(o, 0, NerNum-1) InputNer(o);
	
	fclose(stdin);
}



int win[NerNum];

inline void Game(int player0, int player1)
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
	
	if (Result>0) win[player0]+=2;
	if (Result==0) win[player0]++, win[player1]++;
	if (Result<0) win[player1]+=2;
}

void Compete(int L, int R)
{
	clr(win,0);
	rep(i, L, R) rep(j, L, R) if (i!=j) Game(i,j);
	rep(i, L, R) rep(j, i+1, R) if (win[i]<win[j]) swap(Ner[i],Ner[j]);
	
	int st=L; rep(i, L+1, R+1) if (i>R || win[i]!=win[st])
	{
		if (st!=i-1) Compete(st,i-1);
		st=i;
	}
}

int main()
{
	srand(time(NULL));
	
	scanf("%d", &TIM);
	
	if (TIM) InputData(); else
	{
		rep(i, 0, NerNum-1) NeuronInit(Ner[i]);
	
		OutputData();
	}
	
	
	
	while (TIM++<MaxTIM)
	{
		freopen("con", "w", stdout);
		printf("Round %d:\n", TIM);
		
		rep(i, 1, OPNum) Ner[NerNum-1+i].ActType=-i, Ner[NerNum-1+i].Label=-1;
		
		Compete(0,NerNum+OPNum-1);
		
		freopen("con", "w", stdout); 
		rep(i, 0, NerNum+OPNum-1) printf("%d%c", Ner[i].Label, i==NerNum+OPNum-1?'\n':' '); puts("");
		
		for(int i=0,j=0; i<NerNum; Ner[i++]=Ner[j++]) while (Ner[j].ActType<0) j++;
		
		rep(i, Train1, Train1+Train2-1) NeuronComb(Ner[0], Ner[1], Ner[i], 0.5+rand2()*0.2);
		rep(i, Train1+Train2, Train1+Train2+Train3-1)
		{
			int a=rand()%Train1, b=rand()%Train1; while (a==b) a=rand()%Train1, b=rand()%Train1;
			NeuronComb(Ner[a], Ner[b], Ner[i], 0.5+rand2()*0.2);
		}
		rep(i, Train1+Train2+Train3, Train1+Train2+Train3+Train4-1) Ner[i]=Ner[rand()%Train1];
		rep(i, Train1, Train1+Train2+Train3+Train4-1) NeuronVary(Ner[i],rand01()*3+1.1);
		
		OutputData();
	}
	
	return 0;
}
