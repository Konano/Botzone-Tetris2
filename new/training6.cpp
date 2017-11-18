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

#define NerM 23
#define NerN 18
#define NerNum 20
#define OPNum 1
#define HPinit 30
#define MaxNerNum 5000



inline double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}
inline double rand2(){return rand01()*2-1;}
inline void range(double &a){a+=rand2()/5; a=min(a,1.0); a=max(a,-1.0);}

struct Neuron
{
	double a0[4][3][3], a1[7][4][3][3], w0[43][7], w1[7][7], t[7], b0[4], b1[7], wb0[7], wb1[7], tb;
} Ner[MaxNerNum+9]; int NerCnt, v[NerNum+9], hp[NerNum+9];

inline void NeuronInit(int a){v[a]=++NerCnt, hp[a]=HPinit;}

inline void NeuronVary(Neuron &a, double p)
{
	rep(o, 0, 3) rep(i, 0, 2) rep(j, 0, 2) if (rand01()<p) range(a.a0[o][i][j]);
	rep(o, 0, 6) rep(k, 0, 3) rep(i, 0, 2) rep(j, 0, 2) if (rand01()<p) range(a.a1[o][k][i][j]);
	rep(i, 0, 42) rep(j, 0, 6) if (rand01()<p) range(a.w0[i][j]);
	rep(i, 0, 6) rep(j, 0, 6) if (rand01()<p) range(a.w1[i][j]);
	rep(i, 0, 6) if (rand01()<p) range(a.t[i]);
	
	rep(i, 0, 3) if (rand01()<p) range(a.b0[i]);
	rep(i, 0, 6) if (rand01()<p) range(a.b1[i]);
	rep(i, 0, 6) if (rand01()<p) range(a.wb0[i]);
	rep(i, 0, 6) if (rand01()<p) range(a.wb1[i]);
	if (rand01()<p) range(a.tb);
}

inline void NeuronComb(Neuron &a, Neuron &b, Neuron &c, double p)
{
	rep(o, 0, 3) rep(i, 0, 2) rep(j, 0, 2) c.a0[o][i][j]=(rand01()<p?a:b).a0[o][i][j];
	rep(o, 0, 6) rep(k, 0, 3) rep(i, 0, 2) rep(j, 0, 2) c.a1[o][k][i][j]=(rand01()<p?a:b).a1[o][k][i][j];
	rep(i, 0, 42) rep(j, 0, 6) c.w0[i][j]=(rand01()<p?a:b).w0[i][j];
	rep(i, 0, 6) rep(j, 0, 6) c.w1[i][j]=(rand01()<p?a:b).w1[i][j];
	rep(i, 0, 6) c.t[i]=(rand01()<p?a:b).t[i];
	
	rep(i, 0, 3) c.b0[i]=(rand01()<p?a:b).b0[i];
	rep(i, 0, 6) c.b1[i]=(rand01()<p?a:b).b1[i];
	rep(i, 0, 6) c.wb0[i]=(rand01()<p?a:b).wb0[i];
	rep(i, 0, 6) c.wb1[i]=(rand01()<p?a:b).wb1[i];
	c.tb=(rand01()<p?a:b).tb;
}

inline void OutputNer(int a) 
{
	rep(o, 0, 3) rep(i, 0, 2) rep(j, 0, 2) printf("%.9lf ", Ner[v[a]].a0[o][i][j]);
	rep(o, 0, 6) rep(k, 0, 3) rep(i, 0, 2) rep(j, 0, 2) printf("%.9lf ", Ner[v[a]].a1[o][k][i][j]);
	rep(i, 0, 42) rep(j, 0, 6) printf("%.9lf ", Ner[v[a]].w0[i][j]);
	rep(i, 0, 6) rep(j, 0, 6) printf("%.9lf ", Ner[v[a]].w1[i][j]);
	rep(i, 0, 6) printf("%.9lf ", Ner[v[a]].t[i]);
	
	rep(i, 0, 3) printf("%.9lf ", Ner[v[a]].b0[i]);
	rep(i, 0, 6) printf("%.9lf ", Ner[v[a]].b1[i]);
	rep(i, 0, 6) printf("%.9lf ", Ner[v[a]].wb0[i]);
	rep(i, 0, 6) printf("%.9lf ", Ner[v[a]].wb1[i]);
	printf("%.9lf ", Ner[v[a]].tb);
	puts(""); puts(""); 
}

inline void OutputData()
{
	char filename[16]="train6.txt";
	
	freopen(filename, "w", stdout);
	rep(o, 1, NerNum) printf("%d %d\n", v[o], hp[o]), OutputNer(o);
	fclose(stdout);
}

inline void InputNer(int a) 
{
	scanf("%d%d", &v[a], &hp[a]);
	
	rep(o, 0, 3) rep(i, 0, 2) rep(j, 0, 2) scanf("%lf", &Ner[v[a]].a0[o][i][j]);
	rep(o, 0, 6) rep(k, 0, 3) rep(i, 0, 2) rep(j, 0, 2) scanf("%lf", &Ner[v[a]].a1[o][k][i][j]);
	rep(i, 0, 42) rep(j, 0, 6) scanf("%lf", &Ner[v[a]].w0[i][j]);
	rep(i, 0, 6) rep(j, 0, 6) scanf("%lf", &Ner[v[a]].w1[i][j]);
	rep(i, 0, 6) scanf("%lf", &Ner[v[a]].t[i]);
	
	rep(i, 0, 3) scanf("%lf", &Ner[v[a]].b0[i]);
	rep(i, 0, 6) scanf("%lf", &Ner[v[a]].b1[i]);
	rep(i, 0, 6) scanf("%lf", &Ner[v[a]].wb0[i]);
	rep(i, 0, 6) scanf("%lf", &Ner[v[a]].wb1[i]);
	scanf("%lf", &Ner[v[a]].tb);
}

inline void InputData()
{
	char filename[16]="train6.txt";
	
	freopen(filename, "r", stdin);
	rep(o, 1, NerNum) InputNer(o);
	fclose(stdin);
	
	rep(o, 1, NerNum) NerCnt=max(NerCnt, v[o]);
}

inline int Game(int player0, int player1)
{
	freopen("player0.txt", "w", stdout);
	OutputNer(player0);
	fclose(stdout);
	
	freopen("player1.txt", "w", stdout);
	OutputNer(player1);
	fclose(stdout);
	
	system("final.exe");
	
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

inline int Choose(){int x=rand()%10; return x<3?1:(x<5?2:(x<7?3:x-3));}
inline void Sort(int x){while (x>1 && hp[x-1]<hp[x]) swap(hp[x-1],hp[x]), swap(v[x-1],v[x]), x--;}
inline void Death()
{
	v[NerNum]=++NerCnt, hp[NerNum]=HPinit;
	NeuronComb(Ner[v[Choose()]], Ner[v[Choose()]], Ner[v[NerNum]], 0.5+rand2()*0.2);
	NeuronVary(Ner[v[NerNum]], rand01()/5);
	Sort(NerNum);
}

int TIM=0;
int main()
{
	srand(time(NULL));
	
	int tmp; scanf("%d", &tmp);
	if (tmp) InputData(); else rep(i, 1, NerNum) NeuronInit(i);
	
	int TIM=0; while (NerCnt<=MaxNerNum)
	{
		int A=0, B=0;
		while (A==B) A=rand()%NerNum+1, B=rand()%NerNum+1;
		Game(A,B);
		rep(i, 1, NerNum) Sort(i);
		while (hp[NerNum]<=0) Death();
		
		TIM++; if (TIM%NerNum==0)
		{
			OutputData();
			
			freopen("con", "w", stdout);
			
			OutputSomething();
			
			freopen("history6.txt", "a", stdout);
			
			OutputSomething();
		}
	}
	
	return 0;
}


