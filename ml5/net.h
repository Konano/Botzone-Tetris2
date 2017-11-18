/* 
#include <cstdio>
#include <cmath>
#include <algorithm>

#define rp(i, r) for(int i=0; i<r; i++)
#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))

using namespace std;
*/

#define Learn_Speed 0.8

inline long double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}
inline long double rand2(){return rand01()*2-1;}

class Ner
{
public:
	long double W1[200][30], W2[30][30], W3[30], B1[30], B2[30], B3;
	long double I[200], L1[30], L2[30], Y;
	long double dl1[30], dl2[30], dy;
	
	int TrainCnt;
	
	inline void init()
	{
		TrainCnt=0;
		
		rp(i,200) rp(j,30) W1[i][j]=rand2();
		rp(i,30) rp(j,30) W2[i][j]=rand2();
		rp(i,30) W3[i]=rand2();
		
		rp(i,30) B1[i]=0;
		rp(i,30) B2[i]=0;
		B3=0;
	}
	inline void input()
	{
		freopen("baby.txt", "r", stdin);
		
		scanf("%d", &TrainCnt);
		
		rp(i,200) rp(j,30) scanf("%Lf", &W1[i][j]);
		rp(i,30) rp(j,30) scanf("%Lf", &W2[i][j]);
		rp(i,30) scanf("%Lf", &W3[i]);
		
		rp(i,30) scanf("%Lf", &B1[i]);
		rp(i,30) scanf("%Lf", &B2[i]);
		scanf("%Lf", &B3);
		
		fclose(stdin);
	}
	inline void output()
	{
		freopen("baby.txt", "w", stdout);
		
		printf("%d ", TrainCnt); puts("");
		
		rp(i,200) rp(j,30) printf("%.9Lf ", W1[i][j]); puts("");
		rp(i,30) rp(j,30) printf("%.9Lf ", W2[i][j]); puts("");
		rp(i,30) printf("%.9Lf ", W3[i]); puts("");
		
		rp(i,30) printf("%.9Lf ", B1[i]); puts("");
		rp(i,30) printf("%.9Lf ", B2[i]); puts("");
		printf("%.9Lf ", B3); puts("");
		
		fclose(stdout);
	}
	
	inline long double sigmoid(long double x){return 1.0/(1.0+exp(-x));}
	inline long double max(long double a, double b){return a>b?a:b;}
	inline long double calc()
	{
		long double tmp;
		rp(i,30)
		{
			tmp = B1[i];
			rp(j,200) tmp += I[j] * W1[j][i];
			L1[i] = sigmoid(tmp);
		}
		rp(i,30)
		{
			tmp = B2[i];
			rp(j,30) tmp += L1[j] * W2[j][i];
			L2[i] = sigmoid(tmp);
		}
		
		tmp = B3;
		rp(j,30) tmp += L2[j] * W3[j];
		return Y=sigmoid(tmp);
	}
	
	inline void trainning(long double aim)
	{
		TrainCnt++;
		
		dy = Y*(1-Y)*(aim-Y);
		rp(i,30) dl2[i] = L2[i]*(1-L2[i])*W3[i]*dy;
		rp(i,30) 
		{
			dl1[i] = 0;
			rp(j,30) dl1[i] += W2[i][j]*dl2[j];
			dl1[i] *= L1[i]*(1-L1[i]);
		}
		
		rp(x,200) rp(y,30) W1[x][y]+=Learn_Speed*I[x]*dl1[y];
		rp(y,30) B1[y]+=Learn_Speed*dl1[y];
		rp(x,30) rp(y,30) W2[x][y]+=Learn_Speed*L1[x]*dl2[y];
		rp(y,30) B2[y]+=Learn_Speed*dl2[y];
		rp(x,30) W3[x]+=Learn_Speed*L2[x]*dy;
		B3+=Learn_Speed*dy;
	}
};