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

struct backupNode
{
	// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
	// ��2���������к����һ���������͸��Է���
	int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2];
	 
	// ����ֱ���Է�ת�Ƶ���
	int trans[2][6][MAPWIDTH + 2];
	 
	// ת������
	int transCount[2];
	 
	// ����eliminate��ĵ�ǰ�߶�
	int maxHeight[2];
	 
	// ����ȥ�����ķ���֮��
	int elimTotal[2];

	// �������غϷ�������ȥ��
	int elimCombo[2];
	 
	// ����Ӧ��ҵĸ�������Ŀ�ܼ�
	int typeCountForColor[2][7];
} backupQueue[4]; int backupNum;
 
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
	
	inline bool onTop()
	{
		if (!isValid()) return false;
		rep(i, 0, 3) if (blockY + shape[orientation][i*2+1] == MAPHEIGHT) return true;
		return false;
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
	
	// ����ܷ������ƶ�
	inline bool movedown(){return isValid(-1, blockY - 1);}
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

int erodedPieceCellsMetric, eliminateNum;
int LK2[6][6], LK3[6][6][6], LKnum;
 
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
		
		rep(a1,0,5) rep(a2,0,5) if (!(a1 && a2) && !LK2[a1][a2]) LK2[a1][a2]=LK2[a2][a1]=++LKnum;
		rep(a1,0,5) rep(a2,0,5) rep(a3,0,5) if (!(a1 && a2 && a3) && !LK3[a1][a2][a3]) LK3[a1][a2][a3]=LK3[a3][a2][a1]=++LKnum;
	}
 
	// ��ȥ��
	void eliminate(int color)
	{
		erodedPieceCellsMetric = eliminateNum = 0;
		
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
				eliminateNum++;
				
				for (j = 1; j <= MAPWIDTH; j++)
					erodedPieceCellsMetric += (gridInfo[color][i][j]>>1);
				
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
	
	inline void backup()
	{
		rep(o, 0, 1) rep(i, 1, MAPHEIGHT) rep(j, 1, MAPWIDTH)
			backupQueue[backupNum].gridInfo[o][i][j]=gridInfo[o][i][j];
			
		rep(o, 0, 1) rep(i, 0, 3) rep(j, 1, MAPWIDTH)
			backupQueue[backupNum].trans[o][i][j]=trans[o][i][j];
		
		rep(o, 0, 1)
			backupQueue[backupNum].transCount[o]=transCount[o],
			backupQueue[backupNum].maxHeight[o]=maxHeight[o],
			backupQueue[backupNum].elimTotal[o]=elimTotal[o],
			backupQueue[backupNum].elimCombo[o]=elimCombo[o];
		
		rep(o, 0, 1) rep(i, 0, 6)
			backupQueue[backupNum].typeCountForColor[o][i]=typeCountForColor[o][i];
		
		backupNum++;
	}
	
	inline void recover()
	{
		backupNum--;
		
		rep(o, 0, 1) rep(i, 1, MAPHEIGHT) rep(j, 1, MAPWIDTH)
			gridInfo[o][i][j]=backupQueue[backupNum].gridInfo[o][i][j];
			
		rep(o, 0, 1) rep(i, 0, 3) rep(j, 1, MAPWIDTH)
			trans[o][i][j]=backupQueue[backupNum].trans[o][i][j];
		
		rep(o, 0, 1)
			transCount[o]=backupQueue[backupNum].transCount[o],
			maxHeight[o]=backupQueue[backupNum].maxHeight[o],
			elimTotal[o]=backupQueue[backupNum].elimTotal[o],
			elimCombo[o]=backupQueue[backupNum].elimCombo[o];
		
		rep(o, 0, 1) rep(i, 0, 6)
			typeCountForColor[o][i]=backupQueue[backupNum].typeCountForColor[o][i];
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


#define NerM 23
#define NerN 18

struct Neuron
{
	int ActType;
	double weight[NerM], b0[NerN], theta[NerN];
} Ner;

inline void GetNer()
{

#ifndef _BOTZONE_ONLINE
	freopen("data\\ner.txt", "r", stdin);
#endif

#ifdef _BOTZONE_ONLINE
	freopen("data//ner.txt", "r", stdin);
#endif
	
	rep(i, 0, NerM-1) scanf("%lf", &Ner.weight[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner.b0[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner.theta[i]);
	scanf("%d", &Ner.ActType);
	
	fclose(stdin);
}

inline double Cal(double x, int type)
{
	if (type==-1) return abs(x);
	if (type==0) return (sqrt(x*x+1)-1)/2.0+x;
	if (type==1) return x;
	if (type==2) return 1.0/(1.0+exp(-x));
	if (type==3) return 2.0/(1.0+exp(-x*2.0))-1;
	if (type==4) return atan(x);
	if (type==5) return 1.0/(1.0+fabs(x));
	if (type==6) return x<0?0:x;
	if (type==7) return log(1.0+exp(x));
	return 0;
}

int initLK, initBlankCount, initBlankBig, initBlankSum;

Pii st[309]; bool v[MAPWIDTH+2][MAPHEIGHT+2]; int h[MAPWIDTH+2];

inline int Hmax(int color)
{
	int H=0; clr(h,0);
	rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) if (gridInfo[color][y][x]) h[x]=max(h[x],y), H=max(H,y);
	return H;
}

inline int LK(int color)
{
	clr(v,0); int tot=0, Ans=0;
	rep(x, 1, MAPWIDTH) if (!gridInfo[color][MAPHEIGHT][x]) st[++tot]=Pii(x,MAPHEIGHT), v[x][MAPHEIGHT]=true;
	while (tot)
	{
		int x=st[tot].fi, y=st[tot].se; tot--;
		if (x==1 || gridInfo[color][y][x-1]) Ans+=(y<h[x]); else if (!v[x-1][y]) st[++tot]=Pii(x-1,y), v[x-1][y]=true;
		if (y==1 || gridInfo[color][y-1][x]) Ans+=(y<h[x]); else if (!v[x][y-1]) st[++tot]=Pii(x,y-1), v[x][y-1]=true;
		if (x==MAPWIDTH || gridInfo[color][y][x+1]) Ans+=(y<h[x]); else if (!v[x+1][y]) st[++tot]=Pii(x+1,y), v[x+1][y]=true;
		if (y==MAPHEIGHT || gridInfo[color][y+1][x]) Ans+=(y<h[x]); else if (!v[x][y+1]) st[++tot]=Pii(x,y+1), v[x][y+1]=true;
	}
	return Ans;
}

bool pos[MAPHEIGHT+2];
inline int BlankCount(int color)
{
	int Ans=0; clr(pos,0);
	rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) if (!gridInfo[color][y][x] && y<h[x]) Ans+=(v[x][y]?1:2), pos[y]=true;
	return Ans;
}

inline Pii BlankBig(int color)
{
	int BlankBig=0, BlankSum=0, tot=0;
	rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) if (!gridInfo[color][y][x] && !v[x][y])
	{
		st[tot=1]=Pii(x,y), v[x][y]=true, BlankBig++; int tmp=h[x]-y;
		while (tot)
		{
			int x=st[tot].fi, y=st[tot].se; tot--; tmp=min(tmp, h[x]-y);
			if (x>1 && !gridInfo[color][y][x-1] && !v[x-1][y]) 
				st[++tot]=Pii(x-1,y), v[x-1][y]=true;
			if (y>1 && !gridInfo[color][y-1][x] && !v[x][y-1]) 
				st[++tot]=Pii(x,y-1), v[x][y-1]=true;
			if (x<MAPWIDTH && !gridInfo[color][y][x+1] && !v[x+1][y]) 
				st[++tot]=Pii(x+1,y), v[x+1][y]=true;
			if (y<MAPHEIGHT && !gridInfo[color][y+1][x] && !v[x][y+1]) 
				st[++tot]=Pii(x,y+1), v[x][y+1]=true;
		}
		BlankSum+=tmp;
	}
	return Pii(BlankBig, BlankSum);
}

int CanA, CanB;

inline void Test2(int a, int b, int o)
{
	rep(x, 1, MAPWIDTH-1) if (max(h[x]-a,h[x+1]-b)+o<=MAPHEIGHT) 
		CanB+=(h[x]-a==h[x+1]-b), CanA=min(CanA, abs((h[x]-a)-(h[x+1]-b)));
}
inline int max3(int a, int b, int c){return a>b?max(a,c):max(b,c);}
inline void Test3(int a, int b, int c, int o)
{
	int tmp;
	rep(x, 1, MAPWIDTH-2) if ((tmp=max3(h[x]-a,h[x+1]-b,h[x+2]-c))+o<=MAPHEIGHT) 
		CanB+=(h[x]-a==h[x+1]-b && h[x+1]-b==h[x+2]-c), CanA=min(CanA, (tmp+a-h[x])+(tmp+b-h[x+1])+(tmp+c-h[x+2]));
}
inline Pii CanPlace(int color, int type)
{
	clr(h,0); rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) if (gridInfo[color][y][x]) h[x]=max(h[x],y);
	CanA=100, CanB=0;
	
	if (type==0) Test2(0,0,2), Test2(2,0,2), Test3(0,0,0,1), Test3(0,1,1,1);
	if (type==1) Test2(0,0,2), Test2(0,2,2), Test3(0,0,0,1), Test3(1,1,0,1);
	if (type==2) Test2(1,0,2), Test3(0,0,1,1);
	if (type==3) Test2(0,1,2), Test3(1,0,0,1);
	if (type==4) Test2(1,0,2), Test2(0,1,2), Test3(0,0,0,1), Test3(1,0,1,1);
	if (type==6) Test2(0,0,1);
	
	return Pii(CanA,CanB);
}

inline double Value(int color)
{
	//Util::printField();
	
	double z[NerN];
	
	int a=Hmax(color); 
	z[0]=Ner.weight[0]*a*a;
	z[1]=Ner.weight[1]*(LK(color)-initLK);
	z[2]=Ner.weight[2]*(BlankCount(color)-initBlankCount);
	Pii tmp=BlankBig(color);
	z[3]=Ner.weight[3]*(tmp.first-initBlankBig);
	z[4]=Ner.weight[4]*(tmp.second-initBlankSum);
	z[5]=Ner.weight[5+min(elimCombo[color],3)]*eliminateNum;
	z[6]=Ner.weight[9]*erodedPieceCellsMetric;
	int mn0=-100, mn1=100;
	rep(i, 0, 6) if (i!=5) tmp=CanPlace(color,i), mn0=max(tmp.first,mn0), mn1=min(tmp.second,mn1);
	z[7]=Ner.weight[10]*mn0;
	z[8]=Ner.weight[11]*mn1;
	a=0; rep(x, 1, MAPWIDTH-1) if (abs(h[x]-h[x+1])==0) a++; z[9]=Ner.weight[12]*a;
	a=0; rep(x, 1, MAPWIDTH-1) if (abs(h[x]-h[x+1])==1) a++; z[10]=Ner.weight[13]*a;
	a=0; rep(x, 1, MAPWIDTH-1) if (abs(h[x]-h[x+1])==2) a++; z[11]=Ner.weight[14]*a;
	a=0; rep(x, 1, MAPWIDTH-2) if (abs(h[x]-h[x+1])<=1 && abs(h[x]-h[x+2])<=1 && abs(h[x+1]-h[x+2])<=1) a++; 
	z[12]=Ner.weight[15]*a;
	a=0; h[0]=h[MAPWIDTH+1]=30; rep(x, 1, MAPWIDTH) if (h[x-1]-h[x]>=3 && h[x+1]-h[x]>=3) a++;
	z[17]=Ner.weight[20]*a;
	sort(h+1, h+1+MAPWIDTH); 
	z[13]=Ner.weight[16]*(h[2]-h[1]);
	a=0; rep(y, h[1]+1, min(h[1]+4,MAPHEIGHT)) a+=(!pos[y]); z[14]=Ner.weight[17]*a;
	z[15]=Ner.weight[18]*(!pos[h[1]+1]);
	a=0; rep(y, 1, MAPHEIGHT) a+=(!pos[y]); z[16]=Ner.weight[19]*a;
	
	a=0; rep(o, 0, 6) if (o!=5) a+=typeCountForColor[color][o];
	a=(typeCountForColor[color][5]+2)*6-a;
	z[13]*=Ner.weight[21]*a+Ner.weight[22];
	
	rep(i, 0, NerN-1) z[i]+=Ner.b0[i];
	rep(i, 0, 8) Cal(z[i], Ner.ActType);
	rep(i, 9, 13) Cal(z[i], -1);
	rep(i, 14, NerN-1) Cal(z[i], Ner.ActType);
	double y=0;
	rep(i, 0, NerN-1) y+=z[i]*Ner.theta[i];
	
	return y;
}

/*
Hmax^2
�����ߣ���Ա仯��
����������Ա仯����+ʵ
����ͨ����������Ա仯��
����ⶥmax����Ա仯��
������ - �������غ�
�������������еĸ���
�ɷ������ max(min())
�ɷ��ø��� min()
00 10 20 010 ����
*/




bool vis[MAPWIDTH+2][MAPHEIGHT+2][4];

struct node{int x,y,o;};

inline Tetris Determine(int currBotColor, int type)
{
	Tetris block(type, currBotColor), BestAction(type, currBotColor); BestAction.set(1,1,-1);
	
	Hmax(currBotColor);
	initLK=LK(currBotColor);
	initBlankCount=BlankCount(currBotColor);
	pair<int,int>tmpp=BlankBig(currBotColor);
	initBlankBig=tmpp.first;
	initBlankSum=tmpp.second;
	
	queue<node>q; clr(vis,0);
	
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT-3, MAPHEIGHT) rep(o, 0, 3) if (block.set(x,y,o).onTop())
		vis[x][y][o]=1, q.push((node){x,y,o});
	
	double mx=-1e30, tmp;
	while (!q.empty())
	{
		int x=q.front().x, y=q.front().y, o=q.front().o; q.pop();
		block.set(x,y,o);
		if (block.moveleft() && !vis[x-1][y][o]) vis[x-1][y][o]=1, q.push((node){x-1,y,o});
		if (block.moveright() && !vis[x+1][y][o]) vis[x+1][y][o]=1, q.push((node){x+1,y,o});
		if (block.movedown() && !vis[x][y-1][o]) vis[x][y-1][o]=1, q.push((node){x,y-1,o});
		rep(i, 0, 3) if (block.rotation(i) && !vis[x][y][i]) vis[x][y][i]=1, q.push((node){x,y,i});
		
		if (block.onGround() && Ner.ActType>=0)
		{
			Util::backup(); block.place(); Util::eliminate(currBotColor); // ���У��Է�������
			if ((tmp=Value(currBotColor))>mx) mx=tmp, BestAction.set(block.blockX,block.blockY,block.orientation);
			Util::recover();
		}
	}
	
	if (Ner.ActType<0)
	{
		if (-Ner.ActType==1)
		{
			double mx=-1e30; rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) rep(o, 0, 3) if (block.set(x,y,o).onGround() && vis[x][y][o])
			{
				int lbx[MAPWIDTH+2], lby[MAPHEIGHT+2], h[MAPWIDTH+2]; clr(lbx,0); clr(lby,0); clr(h,0);
				double v=-(y-1);
				
				rep(i, 1, MAPWIDTH) rep(j, 1, MAPHEIGHT) if (gridInfo[currBotColor][j][i]) lbx[i]|=1, lby[j]|=1;
				Util::backup(); block.place(); Util::eliminate(currBotColor); v+=erodedPieceCellsMetric;
				rep(i, 1, MAPWIDTH) dow(j, MAPHEIGHT, 1) if (gridInfo[currBotColor][j][i]) lbx[i]|=2, lby[j]|=2, h[i]=max(h[i],j); else if (h[i]) v-=4.0;
				rep(i, 1, MAPWIDTH) if (lbx[i]!=0 && lbx[i]!=3) v-=1.0;
				rep(i, 1, MAPHEIGHT) if (lby[i]!=0 && lby[i]!=3) v-=1.0;
				h[0]=h[MAPWIDTH+1]=20; rep(i, 1, MAPWIDTH) if (!h[i] && h[i-1] && h[i+1]) v-=min(h[i-1],h[i+1])*(1+min(h[i-1],h[i+1]))/2;
				if (v>mx) mx=v, BestAction.set(block.blockX,block.blockY,block.orientation);
				Util::recover();
			}
		}
	}
	
	return BestAction;
}

inline int WorstDetermine(int color)
{
	// �ٿ������Է�ʲô��
 
	int maxCount = 0, minCount = 99;
	rep(type, 0, 6)
	{
		if (typeCountForColor[color][type] > maxCount)
			maxCount = typeCountForColor[color][type];
		if (typeCountForColor[color][type] < minCount)
			minCount = typeCountForColor[color][type];
	}
	
	int blockForEnemy=0; double mn=1e30, tmp;
	
	rep(type, 0, 6)
	{
		if (typeCountForColor[color][type] == maxCount && maxCount - minCount == 2) continue;
		typeCountForColor[color][type]++;
		
		Tetris block=Determine(color, type); if (block.orientation<0) return type;
		Util::backup(); block.place(); Util::eliminate(color);
		if ((tmp=Value(color))<mn) mn=tmp, blockForEnemy=type;
		Util::recover();
		
		typeCountForColor[color][type]--;
	}
	
	return blockForEnemy;
}
 
 
int main()
{
	// ��������
	istream::sync_with_stdio(false);
	srand(time(NULL));
	//srand(2333);
	Util::init();
 
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
 
 
	int blockForEnemy, finalX, finalY, finalO;
 
	// �������ߣ���ֻ���޸����²��֣�
	
	GetNer();
	
	int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
	
	Tetris currB=Determine(currBotColor, currTypeForColor[currBotColor]);
	Tetris enemyB=Determine(enemyColor, currTypeForColor[enemyColor]);
	
	currB.place(); enemyB.place();
	Util::eliminate(0);
	Util::eliminate(1);
	int tmp=Util::transfer();
	
	if (tmp==-1) blockForEnemy = WorstDetermine(enemyColor); else
	{
		int maxCount = 0, minCount = 99;
		rep(type, 0, 6)
		{
			if (typeCountForColor[enemyColor][type] > maxCount)
				maxCount = typeCountForColor[enemyColor][type];
			if (typeCountForColor[enemyColor][type] < minCount)
				minCount = typeCountForColor[enemyColor][type];
		}
		
		rep(type, 0, 6) if (typeCountForColor[enemyColor][type] != maxCount || maxCount - minCount != 2) 
		{
			blockForEnemy=type;
			break;
		}
	}
	
	finalX=currB.blockX, finalY=currB.blockY, finalO=currB.orientation;
 
	// ���߽���������������ֻ���޸����ϲ��֣�
 
	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;
 
	return 0;
}