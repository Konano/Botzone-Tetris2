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

#define rp(i, r) for(int i=0; i<r; i++)
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












//#include "net.h"





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
	long double w0[2][12][3][3], w1[2][7][12][3][3], w2[142][11], w3[11][11], w4[11], b0[2][12], b1[2][7], b2[11], b3[11], b4;
	long double input0[2][12][22], a0[2][12][10][20], c0[2][12][5][10], a1[2][7][3][8], c1[2][7][8], input1[142], a2[11], a3[11], y;
	long double di0[2][12][22], da0[2][12][10][20], dc0[2][12][5][10], da1[2][7][3][8], dc1[2][7][8], di1[142], da2[11], da3[11], da4;
	
	long double gw0[2][12][3][3], gw1[2][7][12][3][3], gw2[142][11], gw3[11][11], gw4[11], gb0[2][12], gb1[2][7], gb2[11], gb3[11], gb4;
	
	int TrainCnt;
	
	inline void init()
	{
		TrainCnt=0;
		
		rp(t,2) rp(o,12) rp(i,3) rp(j,3) w0[t][o][i][j]=rand2();
		rp(t,2) rp(k,7) rp(o,12) rp(i,3) rp(j,3) w1[t][k][o][i][j]=rand2();
		rp(i,142) rp(j,11) w2[i][j]=rand2();
		rp(i,11) rp(j,11) w3[i][j]=rand2();
		rp(i,11) w4[i]=rand2();
		
		rp(t,2) rp(i,12) b0[t][i]=0;
		rp(t,2) rp(i,7) b1[t][i]=0;
		rp(i,11) b2[i]=0;
		rp(i,11) b3[i]=0;
		b4=0;
	}
	inline void input()
	{
		freopen("baby.txt", "r", stdin);
		
		scanf("%d", &TrainCnt);
		
		rp(t,2) rp(o,12) rp(i,3) rp(j,3) scanf("%Lf", &w0[t][o][i][j]);
		rp(t,2) rp(k,7) rp(o,12) rp(i,3) rp(j,3) scanf("%Lf", &w1[t][k][o][i][j]);
		rp(i,142) rp(j,11) scanf("%Lf", &w2[i][j]);
		rp(i,11) rp(j,11) scanf("%Lf", &w3[i][j]);
		rp(i,11) scanf("%Lf", &w4[i]);
		
		rp(t,2) rp(i,12) scanf("%Lf", &b0[t][i]);
		rp(t,2) rp(i,7) scanf("%Lf", &b1[t][i]);
		rp(i,11) scanf("%Lf", &b2[i]);
		rp(i,11) scanf("%Lf", &b3[i]);
		scanf("%Lf", &b4);
		
		fclose(stdin);
	}
	inline void output()
	{
		freopen("baby.txt", "w", stdout);
		
		printf("%d ", TrainCnt);
		
		rp(t,2) rp(o,12) rp(i,3) rp(j,3) printf("%.9Lf ", w0[t][o][i][j]);
		rp(t,2) rp(k,7) rp(o,12) rp(i,3) rp(j,3) printf("%.9Lf ", w1[t][k][o][i][j]);
		rp(i,142) rp(j,11) printf("%.9Lf ", w2[i][j]);
		rp(i,11) rp(j,11) printf("%.9Lf ", w3[i][j]);
		rp(i,11) printf("%.9Lf ", w4[i]);
		
		rp(t,2) rp(i,12) printf("%.9Lf ", b0[t][i]);
		rp(t,2) rp(i,7) printf("%.9Lf ", b1[t][i]);
		rp(i,11) printf("%.9Lf ", b2[i]);
		rp(i,11) printf("%.9Lf ", b3[i]);
		printf("%.9Lf ", b4);
		
		fclose(stdout);
	}
	
	inline long double sigmoid(long double x){return 1.0/(1.0+exp(-x));}
	inline long double max(long double a, double b){return a>b?a:b;}
	inline long double calc()
	{
		long double tmp;
		rp(t,2) rp(o,12) for(int x=0; x+2<12; x++) for(int y=0; y+2<22; y++)
		{
			tmp = b0[t][o];
			rp(i,3) rp(j,3) tmp += input0[t][x+i][y+j] * w0[t][o][i][j];
			a0[t][o][x][y] = max(tmp,0.0);
		}
		rp(t,2) rp(o,12) rp(x,10) rp(y,20)
		{
			if (x%2==0 && y%2==0) c0[t][o][x/2][y/2]=-1e9;
			c0[t][o][x/2][y/2] = max(c0[t][o][x/2][y/2], a0[t][o][x][y]);
		}
		rp(t,2) rp(o,7) for(int x=0; x+2<5; x++) for(int y=0; y+2<10; y++)
		{
			tmp = b1[t][o];
			rp(k,12) rp(i,3) rp(j,3) tmp += c0[t][k][x+i][y+j] * w1[t][o][k][i][j];
			a1[t][o][x][y] = max(tmp,0.0);
		}
		rp(t,2) rp(o,7) rp(x,3) rp(y,8)
		{
			if (x==0) c1[t][o][y]=-1e9;
			c1[t][o][y] = max(c1[t][o][y], a1[t][o][x][y]);
		}
		rp(t,2) rp(o,7) rp(i,8) input1[56*t+8*o+i] = c1[t][o][i];
		rp(i,11)
		{
			tmp = b2[i];
			rp(j,142) tmp += input1[j] * w2[j][i];
			a2[i] = sigmoid(tmp);
		}
		rp(i,11)
		{
			tmp = b3[i];
			rp(j,11) tmp += a2[j] * w3[j][i];
			a3[i] = sigmoid(tmp);
		}
		
		tmp = b4;
		rp(j,11) tmp += a3[j] * w4[j];
		return y=sigmoid(tmp);
	}
	
	//long double w0[2][12][3][3], w1[2][7][12][3][3], w2[142][11], w3[11][11], w4[11], b0[2][12], b1[2][7], b2[11], b3[11], b4;
	//long double gw0[2][12][3][3], gw1[2][7][12][3][3], gw2[142][11], gw3[11][11], gw4[11], gb0[2][12], gb1[2][7], gb2[11], gb3[11], gb4;
	//long double input0[2][12][22], a0[2][12][10][20], c0[2][12][5][10], a1[2][7][3][8], c1[2][7][8], input1[142], a2[11], a3[11], y;
	//long double da0[2][12][10][20], dc0[2][12][5][10], da1[2][7][3][8], dc1[2][7][8], di1[142], da2[11], da3[11], da4;
	
	inline void trainning(long double aim)
	{
		TrainCnt++;
		da4 = y*(1-y)*(aim-y);
		rp(i,11) da3[i] = a3[i]*(1-a3[i])*w4[i]*da4;
		rp(i,11) 
		{
			da2[i] = 0;
			rp(j,11) da2[i] += w3[i][j]*da3[j];
			da2[i] *= a2[i]*(1-a2[i]);
		}
		rp(i,142)
		{
			di1[i] = 0;
			rp(j,11) di1[i] += w2[i][j]*da2[j];
			di1[i] *= input1[i]*(1-input1[i]);
		}
		rp(t,2) rp(o,7) rp(j,8) dc1[t][o][j] = di1[56*t+8*o+j];
		rp(t,2) rp(o,7) rp(i,3) rp(j,8) da1[t][o][i][j] = (a1[t][o][i][j]==c1[t][o][j] ? dc1[t][o][j] : 0);
		
		rp(t,2) rp(o,12) rp(x,5) rp(y,10) 
		{
			dc0[t][o][x][y] = 0;
			if (c0[t][o][x][y]==0) continue;
			rp(k,7) rp(i,3) rp(j,3) if (x-i>=0 && y-j>=0)
				dc0[t][o][x][y] += da1[t][k][x-i][y-j]*w1[t][k][o][i][j];
		}
		rp(t,2) rp(o,12) rp(i,10) rp(j,20) da0[t][o][i][j] = (a0[t][o][i][j]==c0[t][o][i/2][j/2] ? dc0[t][o][i/2][j/2] : 0);
		
		
		
		
		long double tmp;
		rp(t,2) rp(o,12) rp(x,3) rp(y,3)
		{
			tmp=0;
			rp(i,10) rp(j,20) tmp+=input0[t][i+x][j+y]*da0[t][o][i][j];
			w0[t][o][x][y]-=Learn_Speed*tmp;
		}
		rp(t,2) rp(o,12)
		{
			tmp=0;
			rp(i,10) rp(j,20) tmp+=da0[t][o][i][j];
			b0[t][o]-=Learn_Speed*tmp;
		}
		rp(t,2) rp(o,7) rp(k,12) rp(x,3) rp(y,3)
		{
			tmp=0;
			rp(i,3) rp(j,8) tmp+=c0[t][k][i+x][j+y]*da1[t][o][i][j];
			w1[t][o][k][x][y]-=Learn_Speed*tmp;
		}
		rp(t,2) rp(o,7)
		{
			tmp=0;
			rp(i,3) rp(j,8) tmp+=da1[t][o][i][j];
			b1[t][o]-=Learn_Speed*tmp;
		}
		rp(x,142) rp(y,11) w2[x][y]+=Learn_Speed*input1[x]*da2[y];
		rp(y,11) b2[y]+=Learn_Speed*da2[y];
		rp(x,11) rp(y,11) w3[x][y]+=Learn_Speed*a2[x]*da3[y];
		rp(y,11) b3[y]+=Learn_Speed*da3[y];
		rp(x,11) w4[x]+=Learn_Speed*a3[x]*da4;
		b4+=Learn_Speed*da4;
	}
	
	inline void g(long double aim)
	{
		da4 = y*(1-y)*(aim-y);
		rp(i,11) da3[i] = a3[i]*(1-a3[i])*w4[i]*da4;
		rp(i,11) 
		{
			da2[i] = 0;
			rp(j,11) da2[i] += w3[i][j]*da3[j];
			da2[i] *= a2[i]*(1-a2[i]);
		}
		rp(i,142)
		{
			di1[i] = 0;
			rp(j,11) di1[i] += w2[i][j]*da2[j];
			di1[i] *= input1[i]*(1-input1[i]);
		}
		rp(t,2) rp(o,7) rp(j,8) dc1[t][o][j] = di1[56*t+8*o+j];
		rp(t,2) rp(o,7) rp(i,3) rp(j,8) da1[t][o][i][j] = (a1[t][o][i][j]==c1[t][o][j] ? dc1[t][o][j] : 0);
		
		rp(t,2) rp(o,12) rp(x,5) rp(y,10) 
		{
			dc0[t][o][x][y] = 0;
			if (c0[t][o][x][y]==0) continue;
			rp(k,7) rp(i,3) rp(j,3) if (x-i>=0 && y-j>=0)
				dc0[t][o][x][y] += da1[t][k][x-i][y-j]*w1[t][k][o][i][j];
		}
		rp(t,2) rp(o,12) rp(i,10) rp(j,20) da0[t][o][i][j] = (a0[t][o][i][j]==c0[t][o][i/2][j/2] ? dc0[t][o][i/2][j/2] : 0);
		
		
		
		
		rp(t,2) rp(o,12) rp(x,3) rp(y,3)
		{
			gw0[t][o][x][y]=0;
			rp(i,10) rp(j,20) gw0[t][o][x][y]+=input0[t][i+x][j+y]*da0[t][o][i][j];
		}
		rp(t,2) rp(o,7) rp(k,12) rp(x,3) rp(y,3)
		{
			gw1[t][o][k][x][y]=0;
			rp(i,3) rp(j,8) gw1[t][o][k][x][y]+=c0[t][k][i+x][j+y]*da1[t][o][i][j];
		}
		rp(x,142) rp(y,11) gw2[x][y]=input1[x]*-da2[y];
		rp(x,11) rp(y,11) gw3[x][y]=a2[x]*-da3[y];
		rp(x,11) gw4[x]=a3[x]*-da4;
		
		rp(t,2) rp(o,12)
		{
			gb0[t][o]=0;
			rp(i,10) rp(j,20) gb0[t][o]+=da0[t][o][i][j];
		}
		rp(t,2) rp(o,7)
		{
			gb1[t][o]=0;
			rp(i,3) rp(j,8) gb1[t][o]+=da1[t][o][i][j];
		}
		rp(y,11) gb2[y]=-da2[y];
		rp(y,11) gb3[y]=-da3[y];
		gb4=-da4;
	}
	
	inline bool check(long double &w, long double g)
	{
		w+=1e-4; long double a=calc();
		w-=2e-4; long double b=calc();
		w+=1e-4;
		if ((b-a)/2e-4*g<0) return true;
		//fabs(((b-a)/2e-4)-g)>1e-4 && 
		return false;
	}
	
	inline bool test(long double aim)
	{
		calc(); g(aim);
		
		if (check(b4,gb4)) {puts("b4"); return false;}
		rp(x,11) if (check(w4[x],gw4[x])) {puts("w4"); return false;}
		rp(y,11) if (check(b3[y],gb3[y])) {puts("b3"); return false;}
		rp(x,11) rp(y,11) if (check(w3[x][y],gw3[x][y])) {puts("w3"); return false;}
		rp(y,11) if (check(b2[y],gb2[y])) {puts("b2"); return false;}
		rp(x,142) rp(y,11) if (check(w2[x][y],gw2[x][y])) {puts("w2"); return false;}
		rp(t,2) rp(o,7) if (check(b1[t][o],gb1[t][o])) {puts("b1"); return false;}
		rp(t,2) rp(o,12) if (check(b0[t][o],gb0[t][o])) {puts("b0"); return false;}
		rp(t,2) rp(o,12) rp(x,3) rp(y,3) if (check(w0[t][o][x][y],gw0[t][o][x][y])) {puts("w0"); return false;}
		rp(t,2) rp(o,7) rp(k,12) rp(x,3) rp(y,3) if (check(w1[t][o][k][x][y],gw1[t][o][k][x][y])) {puts("w1"); return false;}
		
		return true;
	}
};


























// ע�⣺x�ķ�Χ��1~MAPWIDTH��y�ķ�Χ��1~MAPHEIGHT
// ���������У�y�����У�c��
// ����ϵ��ԭ�������½�

#define MAPWIDTH 10
#define MAPHEIGHT 20

// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
// ��2���������к����һ���������͸��Է���
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2] = { 0 };

// ����ֱ���Է�ת�Ƶ���
int trans[2][6][MAPWIDTH + 2] = { 0 };

// ת������
int transCount[2] = { 0 };

// ����eliminate��ĵ�ǰ�߶�
int maxHeight[2] = { 0 };

// ����ȥ�����ķ���֮��
int elimTotal[2] = { 0 };

// �������غϷ�������ȥ��
int elimCombo[2] = { 0 };

// һ������ȥ������Ӧ����
const int elimBonus[] = { 0, 1, 3, 5, 7 };

// ����Ӧ��ҵĸ�������Ŀ�ܼ�
int typeCountForColor[2][7] = { 0 };

const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
}; // 7����״(��L| ��L| ��z| ��z| T| ֱһ| ���)��4�ֳ���(��������)��8:ÿ���ڵ������ֱ�Ϊx��y

const int rotateBlank[7][4][10] = {
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { -1,-1,-1,1,1,1,0,0 },{ -1,-1,-1,1,1,-1,0,0 },{ -1,-1,1,1,1,-1,0,0 },{ -1,1,1,1,1,-1,0,0 } },
	{ { 1,-1,-1,1,-2,1,-1,2,-2,2 } ,{ 1,1,-1,-1,-2,-1,-1,-2,-2,-2 } ,{ -1,1,1,-1,2,-1,1,-2,2,-2 } ,{ -1,-1,1,1,2,1,1,2,2,2 } },
	{ { 0,0 },{ 0,0 } ,{ 0,0 } ,{ 0,0 } }
}; // ��ת��ʱ����ҪΪ�յĿ��������ת���ĵ�����


Ner Brain;

class Tetris
{
public:
	const int blockType;   // ��Ƿ������͵���� 0~6
	int blockX;            // ��ת���ĵ�x������
	int blockY;            // ��ת���ĵ�y������
	int orientation;       // ��Ƿ���ĳ��� 0~3
	const int(*shape)[8]; // ��ǰ���ͷ������״����
 
	int color;
 
	Tetris(int t, int color) : blockType(t), shape(blockShape[t]), color(color)
	{ }
 
	inline Tetris &set(int x = -1, int y = -1, int o = -1)
	{
		blockX = x == -1 ? blockX : x;
		blockY = y == -1 ? blockY : y;
		orientation = o == -1 ? orientation : o;
		return *this;
	}
 
	// �жϵ�ǰλ���Ƿ�Ϸ�
	inline bool isValid(int x = -1, int y = -1, int o = -1)
	{
		x = x == -1 ? blockX : x;
		y = y == -1 ? blockY : y;
		o = o == -1 ? orientation : o;
		if (o < 0 || o > 3)
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = x + shape[o][2 * i];
			tmpY = y + shape[o][2 * i + 1];
			if (tmpX < 1 || tmpX > MAPWIDTH ||
				tmpY < 1 || tmpY > MAPHEIGHT ||
				gridInfo[color][tmpY][tmpX] != 0)
				return false;
		}
		return true;
	}
	
	inline bool isValid2(int x = -1, int y = -1, int o = -1)
	{
		x = x == -1 ? blockX : x;
		y = y == -1 ? blockY : y;
		o = o == -1 ? orientation : o;
		if (o < 0 || o > 3)
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = x + shape[o][2 * i];
			tmpY = y + shape[o][2 * i + 1];
			if (tmpX < 1 || tmpX > MAPWIDTH || tmpY < 1) return false;
			if (tmpY > MAPHEIGHT) continue;
			if (gridInfo[color][tmpY][tmpX] != 0) return false;
		}
		return true;
	}
	
	inline bool onTop()
	{
		if (!isValid2()) return false;
		rep(i, 0, 3) if (blockY + shape[orientation][i*2+1] < MAPHEIGHT) return false;
		return true;
	}
 
	// �ж��Ƿ����
	inline bool onGround()
	{
		if (isValid() && !isValid(-1, blockY - 1))
			return true;
		return false;
	}
 
	// ����������ڳ�����
	inline bool place()
	{
		if (!onGround())
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = blockX + shape[orientation][2 * i];
			tmpY = blockY + shape[orientation][2 * i + 1];
			gridInfo[color][tmpY][tmpX] = 2;
		}
		return true;
	}
	
	inline void remove()
	{
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = blockX + shape[orientation][2 * i];
			tmpY = blockY + shape[orientation][2 * i + 1];
			gridInfo[color][tmpY][tmpX] = 0;
		}
	}
	
	// ����ܷ������ƶ�
	inline bool movedown(){return isValid2(-1, blockY - 1);}
	inline bool moveleft(){return isValid(blockX - 1);}
	inline bool moveright(){return isValid(blockX + 1);}
 
	// ����ܷ���ʱ����ת�Լ���o
	inline bool rotation(int o)
	{
		if (o < 0 || o > 3)
			return false;
 
		if (orientation == o)
			return true;
 
		int fromO = orientation;
		int i, blankX, blankY;
		while (true)
		{
			if (!isValid(-1, -1, fromO))
				return false;
 
			if (fromO == o)
				break;
	            
	        // �����ת��ײ
	        for (i = 0; i < 5; i++) {
	            blankX = blockX + rotateBlank[blockType][fromO][2 * i];
	            blankY = blockY + rotateBlank[blockType][fromO][2 * i + 1];
	            if (blankX == blockX && blankY == blockY)
	                break;
	            if (gridInfo[color][blankY][blankX] != 0)
	                return false;
	        }
 
			fromO = (fromO + 1) % 4;
		}
		return true;
	}
};
 
namespace Util
{
	// ΧһȦ���Ǻ�
	inline void init()
	{
		int i;
		for (i = 0; i < MAPHEIGHT + 2; i++)
		{
			gridInfo[1][i][0] = gridInfo[1][i][MAPWIDTH + 1] = -2;
			gridInfo[0][i][0] = gridInfo[0][i][MAPWIDTH + 1] = -2;
		}
		for (i = 0; i < MAPWIDTH + 2; i++)
		{
			gridInfo[1][0][i] = gridInfo[1][MAPHEIGHT + 1][i] = -2;
			gridInfo[0][0][i] = gridInfo[0][MAPHEIGHT + 1][i] = -2;
		}
	}
 
	// ��ȥ��
	void eliminate(int color)
	{
		int &count = transCount[color] = 0;
		int i, j, emptyFlag, fullFlag, firstFull = 1, hasBonus = 0;
		maxHeight[color] = MAPHEIGHT;
		for (i = 1; i <= MAPHEIGHT; i++)
		{
			emptyFlag = 1;
			fullFlag = 1;
			for (j = 1; j <= MAPWIDTH; j++)
			{
				if (gridInfo[color][i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				if (firstFull && ++elimCombo[color] >= 3)
				{
					// ������
					for (j = 1; j <= MAPWIDTH; j++)
						trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					count++;
					hasBonus = 1;
				}
				firstFull = 0;
				for (j = 1; j <= MAPWIDTH; j++)
				{
					// ע������ֻת����ǰ�Ŀ飬���������һ�����µĿ飨���������һ������
					trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					gridInfo[color][i][j] = 0;
				}
				count++;
			}
			else if (emptyFlag)
			{
				maxHeight[color] = i - 1;
				break;
			}
			else
				for (j = 1; j <= MAPWIDTH; j++)
				{
					gridInfo[color][i - count + hasBonus][j] =
						gridInfo[color][i][j] > 0 ? 1 : gridInfo[color][i][j];
					if (count)
						gridInfo[color][i][j] = 0;
				}
		}
		if (count == 0)
			elimCombo[color] = 0;
		maxHeight[color] -= count - hasBonus;
		elimTotal[color] += elimBonus[count];
	}
 
	// ת��˫����ȥ���У�����-1��ʾ���������򷵻�����
	int transfer()
	{
		int color1 = 0, color2 = 1;
		if (transCount[color1] == 0 && transCount[color2] == 0)
			return -1;
		if (transCount[color1] == 0 || transCount[color2] == 0)
		{
			if (transCount[color1] == 0 && transCount[color2] > 0)
				swap(color1, color2);
			int h2;
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			if (h2 > MAPHEIGHT)
				return color2;
			int i, j;
 
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
			return -1;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//��color1���ƶ�count1ȥcolor2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
 
			if (h1 > MAPHEIGHT) return color1;
			if (h2 > MAPHEIGHT) return color2;
 
			int i, j;
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
 
			for (i = h1; i > transCount[color2]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = gridInfo[color1][i - transCount[color2]][j];
 
			for (i = transCount[color2]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = trans[color2][i - 1][j];
 
			return -1;
		}
	}
	
	// ��ӡ�������ڵ���
	inline void printField()
	{
#ifndef _BOTZONE_ONLINE
		static const char *i2s[] = {
			"~~",
			"~~",
			"  ",
			"[]",
			"##"
		};
		cout << "~~��ǽ��[]���飬##���¿�" << endl;
		for (int y = MAPHEIGHT + 1; y >= 0; y--)
		{
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[0][y][x] + 2];
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[1][y][x] + 2];
			cout << endl;
		}
#endif
	}
}

inline void Value(Tetris block, int currBotColor, int ety, int ty)
{
	int enemyColor = 1 - currBotColor;
	
	block.place();
	
	rep(x, 1, 10) rep(y, 1, 20) Brain.input0[0][x][y] = (gridInfo[currBotColor][y][x]==2?0.5:(gridInfo[currBotColor][y][x]==1?1:0));
	rep(x, 1, 10) rep(y, 1, 20) Brain.input0[1][x][y] = (gridInfo[enemyColor][y][x]==2?0.5:(gridInfo[enemyColor][y][x]==1?1:0));
	
	int tot = 112;
	int maxCount = 0, minCount = 99; rep(type, 0, 6)
	{
		maxCount = max(maxCount, typeCountForColor[currBotColor][type]);
		minCount = min(minCount, typeCountForColor[currBotColor][type]);
	}
	rep(type, 0, 6) Brain.input1[tot++] = 1.0*(minCount+2-typeCountForColor[currBotColor][type])/2;
	maxCount = 0, minCount = 99; rep(type, 0, 6)
	{
		maxCount = max(maxCount, typeCountForColor[enemyColor][type]);
		minCount = min(minCount, typeCountForColor[enemyColor][type]);
	}
	rep(type, 0, 6) Brain.input1[tot++] = 1.0*(minCount+2-typeCountForColor[enemyColor][type])/2;
	
	rep(type, 0, 6) Brain.input1[tot++] = (type==ety?1:0);
	rep(type, 0, 6) Brain.input1[tot++] = (type==ty?1:0);
	
	Brain.input1[tot++] = 1.0*min(elimCombo[currBotColor],2)/2;
	Brain.input1[tot++] = 1.0*min(elimCombo[enemyColor],2)/2;
	
	block.remove(); 
	return;
}
 
int main()
{
	// ��������
	istream::sync_with_stdio(false);
	srand(time(NULL));
	Util::init();
	
#ifndef _BOTZONE_ONLINE
	freopen("input.txt", "r", stdin);
#endif
 
	int turnID, blockType, enemyColor, currBotColor;
	int nextTypeForColor[2];
	cin >> turnID;
 
	// �ȶ����һ�غϣ��õ��Լ�����ɫ
	// ˫���ĵ�һ��϶���һ����
	cin >> blockType >> currBotColor;
	enemyColor = 1 - currBotColor;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][blockType]++;
	typeCountForColor[1][blockType]++;
 
	// Ȼ�������ǰÿ�غϵ�������������ָ�״̬
	// ѭ���У�color ��ʾ��ǰ��һ���� color ����Ϊ
	// ƽ̨��֤�������붼�ǺϷ�����
	for (int i = 1; i < turnID; i++)
	{
		int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
		int x, y, o;
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
 
		// �ȶ��Լ��������Ҳ�����Լ�����Ϊ
		// �Լ���������Լ������һ��
		// Ȼ��ģ�����һ�����ÿ�
		cin >> blockType >> x >> y >> o;
 
		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
		myBlock.set(x, y, o).place();
 
		// �Ҹ��Է�ʲô�����ţ�
		typeCountForColor[enemyColor][blockType]++;
		nextTypeForColor[enemyColor] = blockType;
 
		// Ȼ����Լ������룬Ҳ���ǶԷ�����Ϊ
		// ���и��Լ��������ǶԷ������һ��
		cin >> blockType >> x >> y >> o;
 
		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
		enemyBlock.set(x, y, o).place();
 
		// �Է�����ʲô�����ţ�
		typeCountForColor[currBotColor][blockType]++;
		nextTypeForColor[currBotColor] = blockType;
 
		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);
 
		// ����ת��
		Util::transfer();
	}
 
	// �������ߣ���ֻ���޸����²��֣�
	
	Brain.input();
	
	int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1]};
	
	Tetris block(currTypeForColor[currBotColor], currBotColor); block.set(2,6,0);
	
	Value(block,currBotColor,currTypeForColor[enemyColor],0);
	
	return 0;
}