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
	long double w0[12][3][3], w1[7][12][3][3], w2[56][8], w3[8][8], w4[8], b0[12], b1[7], b2[8], b3[8], b4;
	long double input0[12][22], a0[12][10][20], c0[12][5][10], a1[7][3][8], c1[7][8], input1[56], a2[8], a3[8], y;
	long double di0[12][22], da0[12][10][20], dc0[12][5][10], da1[7][3][8], dc1[7][8], di1[56], da2[8], da3[8], da4;
	long double gw0[12][3][3], gw1[7][12][3][3], gw2[56][8], gw3[8][8], gw4[8], gb0[12], gb1[7], gb2[8], gb3[8], gb4;
	
	int TrainCnt;
	
	inline void init()
	{
		TrainCnt=0;
		
		rp(o,12) rp(i,3) rp(j,3) w0[o][i][j]=rand2();
		rp(k,7) rp(o,12) rp(i,3) rp(j,3) w1[k][o][i][j]=rand2();
		rp(i,56) rp(j,8) w2[i][j]=rand2();
		rp(i,8) rp(j,8) w3[i][j]=rand2();
		rp(i,8) w4[i]=rand2();
		
		rp(i,12) b0[i]=0;
		rp(i,7) b1[i]=0;
		rp(i,8) b2[i]=0;
		rp(i,8) b3[i]=0;
		b4=0;
	}
	inline void input()
	{
		freopen("baby.txt", "r", stdin);
		
		scanf("%d", &TrainCnt);
		
		rp(o,12) rp(i,3) rp(j,3) scanf("%Lf", &w0[o][i][j]);
		rp(k,7) rp(o,12) rp(i,3) rp(j,3) scanf("%Lf", &w1[k][o][i][j]);
		rp(i,56) rp(j,8) scanf("%Lf", &w2[i][j]);
		rp(i,8) rp(j,8) scanf("%Lf", &w3[i][j]);
		rp(i,8) scanf("%Lf", &w4[i]);
		
		rp(i,12) scanf("%Lf", &b0[i]);
		rp(i,7) scanf("%Lf", &b1[i]);
		rp(i,8) scanf("%Lf", &b2[i]);
		rp(i,8) scanf("%Lf", &b3[i]);
		scanf("%Lf", &b4);
		
		fclose(stdin);
	}
	inline void output()
	{
		freopen("baby.txt", "w", stdout);
		
		printf("%d ", TrainCnt); puts("");
		
		rp(o,12) rp(i,3) rp(j,3) printf("%.9Lf ", w0[o][i][j]); puts("");
		rp(k,7) rp(o,12) rp(i,3) rp(j,3) printf("%.9Lf ", w1[k][o][i][j]); puts("");
		rp(i,56) rp(j,8) printf("%.9Lf ", w2[i][j]); puts("");
		rp(i,8) rp(j,8) printf("%.9Lf ", w3[i][j]); puts("");
		rp(i,8) printf("%.9Lf ", w4[i]); puts("");
		
		rp(i,12) printf("%.9Lf ", b0[i]); puts("");
		rp(i,7) printf("%.9Lf ", b1[i]); puts("");
		rp(i,8) printf("%.9Lf ", b2[i]); puts("");
		rp(i,8) printf("%.9Lf ", b3[i]); puts("");
		printf("%.9Lf ", b4); puts("");
		
		fclose(stdout);
	}
	
	inline long double sigmoid(long double x){return 1.0/(1.0+exp(-x));}
	inline long double max(long double a, double b){return a>b?a:b;}
	inline long double calc()
	{
		long double tmp;
		rp(o,12) for(int x=0; x+2<12; x++) for(int y=0; y+2<22; y++)
		{
			tmp = b0[o];
			rp(i,3) rp(j,3) tmp += input0[x+i][y+j] * w0[o][i][j];
			a0[o][x][y] = max(tmp,0.0);
		}
		rp(o,12) rp(x,10) rp(y,20)
		{
			if (x%2==0 && y%2==0) c0[o][x/2][y/2]=-1e9;
			c0[o][x/2][y/2] = max(c0[o][x/2][y/2], a0[o][x][y]);
		}
		rp(o,7) for(int x=0; x+2<5; x++) for(int y=0; y+2<10; y++)
		{
			tmp = b1[o];
			rp(k,12) rp(i,3) rp(j,3) tmp += c0[k][x+i][y+j] * w1[o][k][i][j];
			a1[o][x][y] = max(tmp,0.0);
		}
		rp(o,7) rp(x,3) rp(y,8)
		{
			if (x==0) c1[o][y]=-1e9;
			c1[o][y] = max(c1[o][y], a1[o][x][y]);
		}
		rp(o,7) rp(i,8) input1[8*o+i] = c1[o][i];
		rp(i,8)
		{
			tmp = b2[i];
			rp(j,56) tmp += input1[j] * w2[j][i];
			a2[i] = sigmoid(tmp);
		}
		rp(i,8)
		{
			tmp = b3[i];
			rp(j,8) tmp += a2[j] * w3[j][i];
			a3[i] = sigmoid(tmp);
		}
		
		tmp = b4;
		rp(j,8) tmp += a3[j] * w4[j];
		return y=sigmoid(tmp);
	}
	
	inline void trainning(long double aim)
	{
		//long double def=calc();
		
		//if (aim==0) aim=def-0.01;
		
		TrainCnt++;
		
		da4 = y*(1-y)*(aim-y);
		rp(i,8) da3[i] = a3[i]*(1-a3[i])*w4[i]*da4;
		rp(i,8) 
		{
			da2[i] = 0;
			rp(j,8) da2[i] += w3[i][j]*da3[j];
			da2[i] *= a2[i]*(1-a2[i]);
		}
		rp(i,56)
		{
			di1[i] = 0;
			rp(j,8) di1[i] += w2[i][j]*da2[j];
			di1[i] *= input1[i]*(1-input1[i]);
		}
		rp(o,7) rp(j,8) dc1[o][j] = di1[8*o+j];
		rp(o,7) rp(i,3) rp(j,8) da1[o][i][j] = (fabs(a1[o][i][j]-c1[o][j])<1e-12 ? dc1[o][j] : 0);
		
		rp(o,12) rp(x,5) rp(y,10) 
		{
			dc0[o][x][y] = 0;
			if (c0[o][x][y]==0) continue;
			rp(k,7) rp(i,3) rp(j,3) if (x-i>=0 && y-j>=0)
				dc0[o][x][y] += da1[k][x-i][y-j]*w1[k][o][i][j];
		}
		rp(o,12) rp(i,10) rp(j,20) da0[o][i][j] = (fabs(a0[o][i][j]-c0[o][i/2][j/2])<1e-12 ? dc0[o][i/2][j/2] : 0);
		
		
		
		
		long double tmp;
		rp(o,12) rp(x,3) rp(y,3)
		{
			tmp=0;
			rp(i,10) rp(j,20) tmp+=input0[i+x][j+y]*da0[o][i][j];
			w0[o][x][y]-=Learn_Speed*tmp;
		}
		rp(o,12)
		{
			tmp=0;
			rp(i,10) rp(j,20) tmp+=da0[o][i][j];
			b0[o]-=Learn_Speed*tmp;
		}
		rp(o,7) rp(k,12) rp(x,3) rp(y,3)
		{
			tmp=0;
			rp(i,3) rp(j,8) tmp+=c0[k][i+x][j+y]*da1[o][i][j];
			w1[o][k][x][y]-=Learn_Speed*tmp;
		}
		rp(o,7)
		{
			tmp=0;
			rp(i,3) rp(j,8) tmp+=da1[o][i][j];
			b1[o]-=Learn_Speed*tmp;
		}
		rp(x,56) rp(y,8) w2[x][y]+=Learn_Speed*input1[x]*da2[y];
		rp(y,8) b2[y]+=Learn_Speed*da2[y];
		rp(x,8) rp(y,8) w3[x][y]+=Learn_Speed*a2[x]*da3[y];
		rp(y,8) b3[y]+=Learn_Speed*da3[y];
		rp(x,8) w4[x]+=Learn_Speed*a3[x]*da4;
		b4+=Learn_Speed*da4;
	}
};