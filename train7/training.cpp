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

#define NerN 15
#define NerNum 20
#define HPinit 15
#define MaxNerNum 20000
#define Speed 2

inline double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}
inline double rand2(){return rand01()*2-1;}
inline void range(double &a){double tmp=rand2(); while (fabs(tmp)<=0.2) tmp=rand2(); a+=tmp*Speed;}

struct Neuron{double w[5];} Ner[MaxNerNum+9]; int NerCnt, v[NerNum+9], hp[NerNum+9];

inline void NeuronInit(int a)
{
	v[a]=++NerCnt, hp[a]=HPinit;
	rep(i, 0, NerN-1) Ner[v[a]].w[i]=0;
}
inline void NeuronVary(Neuron &a)
{
	rep(i, 0, NerN-1) if (rand01()<0.35) range(a.w[i]);
}
inline void NeuronComb(Neuron &a, Neuron &b, Neuron &c)
{
	rep(i, 0, NerN-1) c.w[i]=(rand01()<0.5?a:b).w[i];
}
inline void OutputNer(int a) 
{
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[v[a]].w[i], i==NerN-1?'\n':' ');
	puts("");
}
inline void OutputData()
{
	freopen("record.dat", "w", stdout);
	rep(o, 1, NerNum) printf("%d %d\n", v[o], hp[o]), OutputNer(o);
	fclose(stdout);
}
inline void InputNer(int a) 
{
	rep(i, 0, NerN-1) scanf("%lf", &Ner[v[a]].w[i]);
}
inline void InputData()
{
	freopen("record.dat", "r", stdin);
	rep(o, 1, NerNum) scanf("%d%d", &v[o], &hp[o]), InputNer(o), NerCnt=max(NerCnt, v[o]);
	fclose(stdin);
}

inline int Game(int player0, int player1)
{
	freopen("ner0.dat", "w", stdout);
	OutputNer(player0);
	fclose(stdout);
	
	freopen("ner1.dat", "w", stdout);
	OutputNer(player1);
	fclose(stdout);
	
	system("VS.exe");
	
	int Result=0;
	freopen("result.dat", "r", stdin);
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

inline int Choose(){int x=rand()%10; return x<3?1:(x<5?2:(x<7?3:x-3));}
inline void Sort(int x){while (x>1 && hp[x-1]<hp[x]) swap(hp[x-1],hp[x]), swap(v[x-1],v[x]), x--;}
inline void Death()
{
	v[NerNum]=++NerCnt, hp[NerNum]=HPinit;
	NeuronComb(Ner[v[Choose()]], Ner[v[Choose()]], Ner[v[NerNum]]);
	NeuronVary(Ner[v[NerNum]]);
	Sort(NerNum);
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
		Game(A,B);
		rep(i, 1, NerNum) Sort(i);
		while (hp[NerNum]<=0) Death();
		
		TIM++; if (TIM%5==0)
		{
			OutputData();
			
			freopen("con", "w", stdout);
			
			OutputSomething();
			
			freopen("history.dat", "a", stdout);
			
			OutputSomething();
		}
	}
	
	return 0;
}