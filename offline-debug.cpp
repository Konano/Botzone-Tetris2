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

int ED;
 
// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
// ��2���������к����һ���������͸��Է���
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2];  // backup
 
// ����ֱ���Է�ת�Ƶ���
int trans[2][4][MAPWIDTH + 2];  // backup
 
// ת������
int transCount[2];  // backup
 
// ����eliminate��ĵ�ǰ�߶�
int maxHeight[2];  // backup
 
// ����ȥ�����ķ���֮��
int elimTotal[2];  // backup

// �������غϷ�������ȥ��
int elimCombo[2];  // backup
 
// һ������ȥ������Ӧ����
const int elimBonus[] = { 0, 1, 3, 5, 7 }; 
 
// ����Ӧ��ҵĸ�������Ŀ�ܼ�
int typeCountForColor[2][7]; // backup
 
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
	int trans[2][4][MAPWIDTH + 2];
	 
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
} backupQueue[3]; int backupNum;

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
 
	// ת��˫����ȥ���У�����0��ʾ���������򷵻�����
	int transfer()
	{
		int color1 = 0, color2 = 1;
		if (transCount[color1] == 0 && transCount[color2] == 0)
			return 0;
		if (transCount[color1] == 0 || transCount[color2] == 0)
		{
			if (transCount[color1] == 0 && transCount[color2] > 0)
				swap(color1, color2);
			int h2;
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			if (h2 > MAPHEIGHT)
				return 1<<color2;
			int i, j;
 
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
			return 0;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//��color1���ƶ�count1ȥcolor2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			
			if (h1 > MAPHEIGHT && h2 > MAPHEIGHT)
			{
				if (elimTotal[color1]!=elimTotal[color2]) 
					return 1<<(elimTotal[color1]<elimTotal[color2]?color1:color2); 
				else 
					return 3;
			}
			if (h1 > MAPHEIGHT) return 1<<color1;
			if (h2 > MAPHEIGHT) return 1<<color2;
 
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
 
			return 0;
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
 
	// ��ɫ�����ܷ������Ϸ
	/* inline bool canPut(int color, int blockType)
	{
		Tetris t(blockType, color);
		for (int y = MAPHEIGHT; y >= 1; y--)
			for (int x = 1; x <= MAPWIDTH; x++)
				for (int o = 0; o < 4; o++)
				{
					t.set(x, y, o);
					if (t.isValid() && checkDirectDropTo(color, blockType, x, y, o))
						return true;
				}
		return false;
	} */
 
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
		rep(i, 0, 6) printf("%d%c", typeCountForColor[0][i], i==6?'\n':' ');
		rep(i, 0, 6) printf("%d%c", typeCountForColor[1][i], i==6?'\n':' ');
#endif
	}
}








#define NerM 11
#define NerN 8

struct Neuron
{
	int ActType;
	double weight[NerM], b0[NerN], theta[NerN];
} Ner[2];

inline void GetNer(int a)
{
	char filename[12]="player0.txt";
	
	filename[6]='0'+a;
	freopen(filename, "r", stdin);
	
	rep(i, 0, NerM-1) scanf("%lf", &Ner[a].weight[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[a].b0[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[a].theta[i]);
	scanf("%d", &Ner[a].ActType);
	
	fclose(stdin);
}

inline double Cal(double x, int type)
{
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

Pii st[309]; bool LKv[MAPWIDTH+2][MAPHEIGHT+2]; int h[MAPWIDTH+2];

inline double Value(int color, int NerID)
{
	Util::printField();
	
	double z[NerN]; clr(z,0); int H=0; clr(h,0); 
	rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) if (gridInfo[color][y][x]) h[x]=max(h[x],y), H=max(H,y);
	z[0]=Ner[NerID].weight[0]*H;
	
	clr(LKv,0); int tot=0, LK=0;
	rep(x, 1, MAPWIDTH) if (!gridInfo[color][MAPHEIGHT][x]) 
		st[++tot]=Pii(x,MAPHEIGHT), LKv[x][MAPHEIGHT]=true; 
	else 
		LK++;
	while (tot)
	{
		int x=st[tot].fi, y=st[tot].se; tot--;
		if (x==1 || gridInfo[color][y][x-1]) LK+=(y<h[x]); else if (!LKv[x-1][y]) st[++tot]=Pii(x-1,y), LKv[x-1][y]=true;
		if (y==1 || gridInfo[color][y-1][x]) LK+=(y<h[x]); else if (!LKv[x][y-1]) st[++tot]=Pii(x,y-1), LKv[x][y-1]=true;
		if (x==MAPWIDTH || gridInfo[color][y][x+1]) LK+=(y<h[x]); else if (!LKv[x+1][y]) st[++tot]=Pii(x+1,y), LKv[x+1][y]=true;
		if (y==MAPHEIGHT || gridInfo[color][y+1][x]) LK+=(y<h[x]); else if (!LKv[x][y+1]) st[++tot]=Pii(x,y+1), LKv[x][y+1]=true;
	}
	z[2]=Ner[NerID].weight[2]*LK;
	
	int Blank=0, BlankBig=0, BlankTop=0, tmp;
	rep(x, 1, MAPWIDTH) rep(y, 1, MAPHEIGHT) if (!gridInfo[color][y][x] && !LKv[x][y])
	{
		st[tot=1]=Pii(x,y), LKv[x][y]=true, BlankBig++, tmp=h[x]-y;
		while (tot)
		{
			Blank++; tmp=min(tmp, h[x]-y);
			int x=st[tot].fi, y=st[tot].se; tot--;
			if (x>1 && !gridInfo[color][y][x-1] && !LKv[x-1][y]) 
				st[++tot]=Pii(x-1,y), LKv[x-1][y]=true;
			if (y>1 && !gridInfo[color][y-1][x] && !LKv[x][y-1]) 
				st[++tot]=Pii(x,y-1), LKv[x][y-1]=true;
			if (x<MAPWIDTH && !gridInfo[color][y][x+1] && !LKv[x+1][y]) 
				st[++tot]=Pii(x+1,y), LKv[x+1][y]=true;
			if (y<MAPHEIGHT && !gridInfo[color][y+1][x] && !LKv[x][y+1]) 
				st[++tot]=Pii(x,y+1), LKv[x][y+1]=true;
		}
		BlankTop=max(BlankTop, tmp);
	}
	z[3]=Ner[NerID].weight[3]*Blank;
	z[4]=Ner[NerID].weight[4]*BlankBig;
	z[5]=Ner[NerID].weight[5]*BlankTop;
	
	sort(h+1, h+1+MAPWIDTH);
	double ave=0; rep(i, 2, MAPWIDTH) ave+=h[i]; ave/=(MAPWIDTH-1);
	double sum=0; rep(i, 2, MAPWIDTH) sum+=(1.0*h[i]-ave)*(1.0*h[i]-ave); sum/=(MAPWIDTH-1);
	z[1]=Ner[NerID].weight[1]*sum;
	z[6]=Ner[NerID].weight[6+min(elimCombo[color],3)]*eliminateNum;
	z[7]=Ner[NerID].weight[10]*erodedPieceCellsMetric;
	
	
	rep(i, 0, NerN-1) z[i]+=Ner[NerID].b0[i];
	rep(i, 0, NerN-1) Cal(z[i], Ner[NerID].ActType);
	double y=0;
	rep(i, 0, NerN-1) y+=z[i]*Ner[NerID].theta[i];
	
	return y;
}

/*
Hmax
H�����ȥ���һ����
������
������
����ͨ������
����ⶥmax
������
�������غ�
�������������еĸ���
*/







int blockType, typePosX, typePosY, typePosO, blockForEnemy, nextTypeForColor[2], currTypeForColor[2];

struct node{int x,y,o;};

bool vis[MAPWIDTH+2][MAPHEIGHT+2][4];

inline Tetris Determine(int currBotColor, int type, int NerColor)
{
	
	Tetris block(type, currBotColor), BestAction(type, currBotColor); BestAction.set(1,1,-1);
	
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
		
		if (block.onGround() && Ner[NerColor].ActType>=0)
		{
			Util::backup(); block.place(); Util::eliminate(currBotColor); // ���У��Է�������
			if ((tmp=Value(currBotColor,NerColor))>mx) mx=tmp, BestAction.set(block.blockX,block.blockY,block.orientation);
			Util::recover();
		}
	}
	
	if (Ner[NerColor].ActType<0)
	{
		if (-Ner[NerColor].ActType==1)
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
	/* 	if (-Ner[NerColor].ActType==2)
		{
			
		}
		if (-Ner[NerColor].ActType==3)
		{
			
		}
		if (-Ner[NerColor].ActType==4)
		{
			
		} */
	}
	
	return BestAction;
}

inline void WorstDetermine(int color)
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
	
	blockForEnemy=0; double mn=1e30, tmp;
	
	rep(type, 0, 6)
	{
		if (typeCountForColor[color][type] == maxCount && maxCount - minCount == 2) continue;
		Tetris block=Determine(color, type, color^1); if (block.orientation<0) {blockForEnemy=type; return;}
		Util::backup(); block.place(); Util::eliminate(color);
		if ((tmp=Value(color,color^1))<mn) mn=tmp, blockForEnemy=type;
		Util::recover();
	}
}

inline void PlayerAction(int currBotColor)
{
	int enemyColor=1^currBotColor;
	
	Tetris currB=Determine(currBotColor, currTypeForColor[currBotColor],currBotColor);
	Tetris enemyB=Determine(enemyColor, currTypeForColor[enemyColor],currBotColor);
	
	Util::backup();
	
	currB.place(); enemyB.place();
	Util::eliminate(0);
	Util::eliminate(1);
	Util::transfer();
	
	WorstDetermine(enemyColor);
	
	Util::recover();
	
	typePosX=currB.blockX, typePosY=currB.blockY, typePosO=currB.orientation;
}

inline int TestCanPlace()
{
	int fg=0;
	
	Tetris block0(currTypeForColor[0],0);
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT-3, MAPHEIGHT) rep(o, 0, 3) if (block0.set(x,y,o).onTop()) fg|=1;
	Tetris block1(currTypeForColor[1],1);
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT-3, MAPHEIGHT) rep(o, 0, 3) if (block1.set(x,y,o).onTop()) fg|=2;
	
	fg^=3; if (fg==3 && elimTotal[0]!=elimTotal[1]) fg=(elimTotal[0]<elimTotal[1]?1:2);
	
	return fg;
}




 
int main()
{
	GetNer(0); GetNer(1); freopen("con", "r", stdin);

	istream::sync_with_stdio(false);
	srand(time(NULL)); printf("%lld\n", time(NULL));
	//srand(1508691595);
	Util::init(); // Χ���Ǻ�
	
	blockType = rand()%7;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][blockType]++;
	typeCountForColor[1][blockType]++;
	
	int turnID = 0; while (++turnID)
	{
		currTypeForColor[0] = nextTypeForColor[0];
		currTypeForColor[1] = nextTypeForColor[1];
		
		if ((ED=TestCanPlace())) goto Final;
 
		// �ȶ��Լ��������Ҳ�����Լ�����Ϊ
		// �Լ���������Լ������һ��
		// Ȼ��ģ�����һ�����ÿ�
		PlayerAction(0);
 
		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[0], 0);
		myBlock.set(typePosX, typePosY, typePosO);
 
		// �Ҹ��Է�ʲô�����ţ�
		nextTypeForColor[1] = blockForEnemy;
 
		// Ȼ����Լ������룬Ҳ���ǶԷ�����Ϊ
		// ���и��Լ��������ǶԷ������һ��
		PlayerAction(1);
 
		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[1], 1);
		enemyBlock.set(typePosX, typePosY, typePosO);
 
		// �Է�����ʲô�����ţ�
		nextTypeForColor[0] = blockForEnemy;
		
		myBlock.place();
		enemyBlock.place();
		typeCountForColor[0][nextTypeForColor[0]]++;
		typeCountForColor[1][nextTypeForColor[1]]++;
		
		Util::printField();
		system("pause");
 
		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);
 
		// ����ת��
		if ((ED=Util::transfer())) goto Final;
		
		
	}
	
	//freopen("result.txt", "w", stdout);
	Final:if (ED==1) puts("Right Win!");
	if (ED==2) puts("Left Win!");
	if (ED==3) puts("No one win. QwQ");
	system("pause");
	//fclose(stdout);
 
	return 0;
}