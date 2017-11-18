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
#define makePPP(a,b,c,d) make_pair(make_pair(a,b),make_pair(c,d))

typedef long long ll;
typedef pair<int,int> Pii;
typedef pair<ll,int> Pli;
typedef pair<double,int> Pdi;
typedef long double ld;
typedef unsigned long long ull;
typedef pair<pair<int,int>,pair<int,int> > PPP;

// ע�⣺x�ķ�Χ��1~MAPWIDTH��y�ķ�Χ��1~MAPHEIGHT
// ���������У�y�����У�c��
// ����ϵ��ԭ�������½�

#define MAPWIDTH 10
#define MAPHEIGHT 20

#define MaxGame 20000
#define MCTS_Cnt 200

int ED;

inline double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}

// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
// ��2���������к����һ���������͸��Է���
int gridInfo[MaxGame][2][MAPHEIGHT + 2][MAPWIDTH + 2] = { 0 };

// ����ֱ���Է�ת�Ƶ���
int trans[2][6][MAPWIDTH + 2] = { 0 };

// ת������
int transCount[2] = { 0 };

// ����eliminate��ĵ�ǰ�߶�
int maxHeight[2] = { 0 };

// ����ȥ�����ķ���֮��
int elimTotal[MaxGame][2] = { 0 };

// �������غϷ�������ȥ��
int elimCombo[MaxGame][2] = { 0 };

// ����Ӧ��ҵĸ�������Ŀ�ܼ�
int typeCountForColor[MaxGame][2][7] = { 0 };

int gameCnt = 1, nowGame = 0;

// һ������ȥ������Ӧ����
const int elimBonus[] = { 0, 1, 3, 5, 7 };

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
				gridInfo[nowGame][color][tmpY][tmpX] != 0)
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
			if (gridInfo[nowGame][color][tmpY][tmpX] != 0) return false;
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
			gridInfo[nowGame][color][tmpY][tmpX] = 2;
		}
		return true;
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
	            if (gridInfo[nowGame][color][blankY][blankX] != 0)
	                return false;
	        }
 
			fromO = (fromO + 1) % 4;
		}
		return true;
	}
	
	inline double landingHeight()
	{
		int mn=21, mx=0;
		rep(i, 0, 3) mn=min(mn, blockY+shape[orientation][2*i+1]), mx=max(mx, blockY+shape[orientation][2*i+1]);
		return 1.0*(mn+mx)/2;
	}
};

int eliminateUnit, eliminateRow;

namespace Util
{
	// ΧһȦ���Ǻ�
	inline void init()
	{
		int i;
		for (i = 0; i < MAPHEIGHT + 2; i++)
		{
			gridInfo[0][1][i][0] = gridInfo[0][1][i][MAPWIDTH + 1] = -2;
			gridInfo[0][0][i][0] = gridInfo[0][0][i][MAPWIDTH + 1] = -2;
		}
		for (i = 0; i < MAPWIDTH + 2; i++)
		{
			gridInfo[0][1][0][i] = gridInfo[0][1][MAPHEIGHT + 1][i] = -2;
			gridInfo[0][0][0][i] = gridInfo[0][0][MAPHEIGHT + 1][i] = -2;
		}
	}
 
	// ��ȥ��
	void eliminate(int color)
	{
		eliminateUnit = eliminateRow = 0;
		
		int &count = transCount[color] = 0;
		int i, j, emptyFlag, fullFlag, firstFull = 1, hasBonus = 0;
		maxHeight[color] = MAPHEIGHT;
		for (i = 1; i <= MAPHEIGHT; i++)
		{
			emptyFlag = 1;
			fullFlag = 1;
			for (j = 1; j <= MAPWIDTH; j++)
			{
				if (gridInfo[nowGame][color][i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				eliminateRow++;
				
				for (j = 1; j <= MAPWIDTH; j++)
					eliminateUnit += (gridInfo[nowGame][color][i][j]>>1);
				
				if (firstFull && ++elimCombo[nowGame][color] >= 3)
				{
					// ������
					for (j = 1; j <= MAPWIDTH; j++)
						trans[color][count][j] = gridInfo[nowGame][color][i][j] == 1 ? 1 : 0;
					count++;
					hasBonus = 1;
					
					eliminateRow++;
				}
				firstFull = 0;
				for (j = 1; j <= MAPWIDTH; j++)
				{
					// ע������ֻת����ǰ�Ŀ飬���������һ�����µĿ飨���������һ������
					trans[color][count][j] = gridInfo[nowGame][color][i][j] == 1 ? 1 : 0;
					gridInfo[nowGame][color][i][j] = 0;
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
					gridInfo[nowGame][color][i - count + hasBonus][j] =
						gridInfo[nowGame][color][i][j] > 0 ? 1 : gridInfo[nowGame][color][i][j];
					if (count)
						gridInfo[nowGame][color][i][j] = 0;
				}
		}
		if (count == 0)
			elimCombo[nowGame][color] = 0;
		maxHeight[color] -= count - hasBonus;
		elimTotal[nowGame][color] += elimBonus[count - hasBonus];
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
				return ED=(1<<color2),color2;
			int i, j;
 
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[nowGame][color2][i][j] = gridInfo[nowGame][color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[nowGame][color2][i][j] = trans[color1][i - 1][j];
			return -1;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//��color1���ƶ�count1ȥcolor2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
 
			if (h1 > MAPHEIGHT) return ED=(1<<color1),color1;
			if (h2 > MAPHEIGHT) return ED=(1<<color2),color2;
 
			int i, j;
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[nowGame][color2][i][j] = gridInfo[nowGame][color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[nowGame][color2][i][j] = trans[color1][i - 1][j];
 
			for (i = h1; i > transCount[color2]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[nowGame][color1][i][j] = gridInfo[nowGame][color1][i - transCount[color2]][j];
 
			for (i = transCount[color2]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[nowGame][color1][i][j] = trans[color2][i - 1][j];
 
			return -1;
		}
	}
	
	inline int fork()
	{
		int g = gameCnt++;
		
		rep(o, 0, 1) rep(i, 0, MAPHEIGHT+1) rep(j, 0, MAPWIDTH+1)
			gridInfo[g][o][i][j]=gridInfo[nowGame][o][i][j];
		
		rep(o, 0, 1)
			elimTotal[g][o]=elimTotal[nowGame][o],
			elimCombo[g][o]=elimCombo[nowGame][o];
		
		rep(o, 0, 1) rep(i, 0, 6)
			typeCountForColor[g][o][i]=typeCountForColor[nowGame][o][i];
		
		return g;
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
				cout << i2s[gridInfo[nowGame][0][y][x] + 2];
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[nowGame][1][y][x] + 2];
			cout << endl;
		}
#endif
	}
}

namespace Feature
{
	int maxHeight;
	int erodedPieceCellsMetric;
	int boardRowTransitions;
	int boardColTransitions;
	int boardBuriedHoles;
	int boardWells;
	int boradHoleDepth;
	int boradHoleRow;
	
	inline void GetFeature(int color)
	{
		const int(*grid)[MAPWIDTH + 2] = gridInfo[nowGame][color];
		maxHeight = boardRowTransitions = boardColTransitions = boardBuriedHoles = boardWells = boradHoleDepth = boradHoleRow = 0;
		
		erodedPieceCellsMetric = eliminateUnit*eliminateRow;
		
		bool fg; int W, L; int rowHole = 0;
		rep(x, 1, 10)
		{
			fg=false; W=0; L=0;
			dow(y, 20, 1)
			{
				if (grid[y][x]) 
					fg=true, maxHeight = max(maxHeight,y), W = 0, L++;
				else
				{
					boardRowTransitions += (grid[y][x-1]?1:0) + (grid[y][x+1]?1:0);
					boardColTransitions += (grid[y-1][x]?1:0) + (y!=20&&grid[y+1][x]?1:0);
					
					boardBuriedHoles += fg;
					rowHole |= ((fg)<<y);
					
					if (grid[y][x-1] && grid[y][x+1]) W++;
					boardWells += W;
					
					boradHoleDepth += L;
					L = 0;
				}
			}
		}
		while (rowHole) rowHole -= lowbit(rowHole), boradHoleRow++;
	}
}

inline double Value(Tetris block, int currBotColor)
{
	double value=0; int tmp=nowGame;
	value += -1.262900e+01 * block.landingHeight();
	
	nowGame=Util::fork(); block.place(); Util::eliminate(currBotColor); Feature::GetFeature(currBotColor);
	
	value += +6.601974e+00 * Feature::erodedPieceCellsMetric;
	value += -9.215815e+00 * Feature::boardRowTransitions;
	value += -1.977356e+01 * Feature::boardColTransitions;
	value += -1.308335e+01 * Feature::boardBuriedHoles;
	value += -1.048747e+01 * Feature::boardWells;
	value += -1.611863e+00 * Feature::boradHoleDepth;
	value += -2.404087e+01 * Feature::boradHoleRow;
	
	nowGame=tmp, gameCnt--;
	return value;
}

struct act{int x,y,o; double v,v2; int w,l;} g[MaxGame*10], h[MaxGame*10];
struct act2{int ty; double v2; int w,l;} g2[MaxGame*7], h2[MaxGame*7];
bool cmp(act a, act b){return a.v>b.v;}
int ST[MaxGame], ST2[MaxGame], eST[MaxGame], eST2[MaxGame], Over[MaxGame], actCnt, curType, eneType, cur, ene;
map<PPP,int> NEXT, A;

bool vis[MAPWIDTH+2][MAPHEIGHT+4][4];
struct node{int x,y,o;}; queue<node>q;
inline void GetAllAction(int cur, int type, act *a, int &st, act2 *b, int &st2)
{
	//Util::printField();
	int ene = 1-cur, maxCount = 0, minCount = 99;
	rep(ty, 0, 6)
	{
		if (typeCountForColor[nowGame][ene][ty] > maxCount)
			maxCount = typeCountForColor[nowGame][ene][ty];
		if (typeCountForColor[nowGame][ene][ty] < minCount)
			minCount = typeCountForColor[nowGame][ene][ty];
	}
	rep(ty, 0, 6) if (typeCountForColor[nowGame][ene][ty] != maxCount || maxCount - minCount != 2) b[st2++]=(act2){ty, 0, 0, 0};
	
	int tmp = st; Tetris block(type, cur); clr(vis,0);
	
	if (type==0 || type==1 || type==4)
		rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT, MAPHEIGHT+2) rep(o, 0, 3)
			if (block.set(x,y,o).onTop()) vis[x][y][o]=1, q.push((node){x,y,o});
	
	if (type==2 || type==3 || type==5)
		rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT, MAPHEIGHT+2) rep(o, 0, 1)
			if (block.set(x,y,o).onTop()) vis[x][y][o]=1, q.push((node){x,y,o});
	
	if (type==6)
		rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT, MAPHEIGHT+2)
			if (block.set(x,y,0).onTop()) vis[x][y][0]=1, q.push((node){x,y,0});
	
	while (!q.empty())
	{
		int x=q.front().x, y=q.front().y, o=q.front().o; q.pop();
		block.set(x,y,o);
		if (!vis[x-1][y][o] && block.moveleft()) vis[x-1][y][o]=1, q.push((node){x-1,y,o});
		if (!vis[x+1][y][o] && block.moveright()) vis[x+1][y][o]=1, q.push((node){x+1,y,o});
		if (!vis[x][y-1][o] && block.movedown()) vis[x][y-1][o]=1, q.push((node){x,y-1,o});
		
		if (type==0 || type==1 || type==4)
			rep(i, 0, 3) if (!vis[x][y][i] && block.rotation(i)) vis[x][y][i]=1, q.push((node){x,y,i});
		
		if (type==2 || type==3 || type==5)
			if (!vis[x][y][1-o] && block.rotation(1-o)) vis[x][y][1-o]=1, q.push((node){x,y,1-o});
		
		if (block.onGround()) a[st++] = (act){block.blockX, block.blockY, block.orientation, Value(block, cur), 0, 0, 0};
	}
	
	sort(a+tmp, a+st, cmp);
	
	int x=min(3,st-tmp); while (x!=st-tmp && a[tmp+x-1].v == a[tmp+x].v) x++;
	st=tmp+x; dow(o, st-1, tmp) a[o].v=(a[o].v!=a[tmp].v ? 0.8 : 1);
}

double v2sum, v2ave; int v2cnt, v2type; bool fuckyou;
inline double Value2()
{
	double vcur=0; Feature::GetFeature(cur);
	vcur += -1.262900e+01 * Feature::maxHeight;
	vcur += +6.601974e+00 * Feature::erodedPieceCellsMetric;
	vcur += -9.215815e+00 * Feature::boardRowTransitions;
	vcur += -1.977356e+01 * Feature::boardColTransitions;
	vcur += -1.308335e+01 * Feature::boardBuriedHoles;
	vcur += -1.048747e+01 * Feature::boardWells;
	vcur += -1.611863e+00 * Feature::boradHoleDepth;
	vcur += -2.404087e+01 * Feature::boradHoleRow;
	
	double vene=0; Feature::GetFeature(ene);
	vene += -1.262900e+01 * Feature::maxHeight;
	vene += +6.601974e+00 * Feature::erodedPieceCellsMetric;
	vene += -9.215815e+00 * Feature::boardRowTransitions;
	vene += -1.977356e+01 * Feature::boardColTransitions;
	vene += -1.308335e+01 * Feature::boardBuriedHoles;
	vene += -1.048747e+01 * Feature::boardWells;
	vene += -1.611863e+00 * Feature::boradHoleDepth;
	vene += -2.404087e+01 * Feature::boradHoleRow;
	
	return vcur-vene;
}

inline int Choose(act *a, int L, int R)
{
	if (!v2type) return L+rand()%(R+1-L);
	double sum=0; rep(i, L, R) 
		a[i].v2 = a[i].v/(1+log2(actCnt?actCnt:1)) + (a[i].w+a[i].l ? 1.0*a[i].w/(a[i].w+a[i].l) : 0.5) + sqrt(2*log(actCnt?actCnt:1)/(a[i].w+a[i].l ? a[i].w+a[i].l : 0.5)), sum+=a[i].v2;
	double tmp=rand01()*sum; 
	int x=L; while (tmp>a[x].v2) tmp-=a[x++].v2;
	return x;
}
inline int Choose(act2 *a, int L, int R)
{
	if (!v2type) return L+rand()%(R+1-L);
	double sum=0; rep(i, L, R) 
		a[i].v2 = (a[i].w+a[i].l ? 1.0*a[i].w/(a[i].w+a[i].l) : 0.5) + sqrt(2*log(actCnt?actCnt:1)/(a[i].w+a[i].l ? a[i].w+a[i].l : 0.5)), sum+=a[i].v2;
	double tmp=rand01()*sum; 
	int x=L; while (tmp>a[x].v2) tmp-=a[x++].v2;
	return x;
}

bool MCTS(int layer)
{
	if (actCnt == 1 && nowGame == gameCnt-1)
	{
		GetAllAction(cur, curType, g, ST[nowGame+1], g2, ST2[nowGame+1]);
		if (ST[nowGame]==ST[nowGame+1]) ED|=(1<<cur);
		
		GetAllAction(ene, eneType, h, eST[nowGame+1], h2, eST2[nowGame+1]);
		if (eST[nowGame]==eST[nowGame+1]) ED|=(1<<ene);
		
		Over[nowGame]=ED;
		if (ED==3) return elimTotal[nowGame][cur] > elimTotal[nowGame][ene]; else if (ED) return ED==(1<<cur)?0:1;
	}
	
	int curAct = Choose(g, ST[nowGame], ST[nowGame+1]-1);
	int eneNext = Choose(g2, ST2[nowGame], ST2[nowGame+1]-1);
	int eneAct = Choose(h, eST[nowGame], eST[nowGame+1]-1);
	int curNext = Choose(h2, eST2[nowGame], eST2[nowGame+1]-1);
	
	//if (nowGame==0) fuckyou = (curAct==0);
	
	if (NEXT[makePPP(curAct, eneNext, eneAct, curNext)]) 
		nowGame = NEXT[makePPP(curAct, eneNext, eneAct, curNext)]; 
	else
	{
		nowGame = NEXT[makePPP(curAct, eneNext, eneAct, curNext)] = Util::fork();
		
		ST[nowGame+1]=ST[nowGame], ST2[nowGame+1]=ST2[nowGame], 
		eST[nowGame+1]=eST[nowGame], eST2[nowGame+1]=eST2[nowGame];
		
		Tetris curBlock(curType, cur);
		curBlock.set(g[curAct].x, g[curAct].y, g[curAct].o).place();
		Tetris eneBlock(eneType, ene);
		eneBlock.set(h[eneAct].x, h[eneAct].y, h[eneAct].o).place();
		typeCountForColor[nowGame][ene][g2[eneNext].ty]++;
		typeCountForColor[nowGame][cur][h2[curNext].ty]++;
		
		//if (fuckyou) Util::printField();
		
		Util::eliminate(0);
		Util::eliminate(1);
		if (Util::transfer()!=-1) Over[nowGame]=ED;
	}
	
	if (layer == 3)
	{
		if (!v2type)
		{
			v2cnt++;
			if (!ED) v2sum+=Value2();
			if (ED==(1<<cur)) v2sum-=1000;
			if (ED==(1<<ene)) v2sum+=1000;
			return false;
		}
		else if (Value2()>=v2ave)
			Over[nowGame]=ED=(1<<ene);
		else
			Over[nowGame]=ED=(1<<cur);
	}
	
	if (Over[nowGame])
	{
		if (Over[nowGame] == (1<<ene))
			return g[curAct].w+=v2type,g2[eneNext].w+=v2type,h[eneAct].l+=v2type,h2[curNext].l+=v2type,true;
		else 
			return g[curAct].l+=v2type,g2[eneNext].l+=v2type,h[eneAct].w+=v2type,h2[curNext].w+=v2type,false; 
	}
	
	actCnt = (v2type ? ++A[makePPP(curAct, eneNext, eneAct, curNext)] : 1);
	curType = h2[curNext].ty;
	eneType = g2[eneNext].ty;
	
	if (MCTS(layer+1)) 
		return g[curAct].w+=v2type,g2[eneNext].w+=v2type,h[eneAct].l+=v2type,h2[curNext].l+=v2type,true;
	else 
		return g[curAct].l+=v2type,g2[eneNext].l+=v2type,h[eneAct].w+=v2type,h2[curNext].w+=v2type,false;
}

int main()
{
	clock_t startTime = clock();
	
	// ��������
	istream::sync_with_stdio(false);
	//srand(time(NULL));
	srand(2333);
	Util::init();
	
#ifndef _BOTZONE_ONLINE
	freopen("input0.txt", "r", stdin);
#endif
 
	int blockType, enemyColor, currBotColor;
	int nextTypeForColor[2];
 
	// �ȶ����һ�غϣ��õ��Լ�����ɫ
	// ˫���ĵ�һ��϶���һ����
	scanf("%d%d", &blockType, &currBotColor);
	enemyColor = 1 - currBotColor;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][0][blockType]++;
	typeCountForColor[0][1][blockType]++;
 
	// Ȼ�������ǰÿ�غϵ�������������ָ�״̬
	// ѭ���У�color ��ʾ��ǰ��һ���� color ����Ϊ
	// ƽ̨��֤�������붼�ǺϷ�����
	int x, y, o; while (scanf("%d%d%d%d", &blockType, &x, &y, &o)!=EOF)
	{
		int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
 
		// �ȶ��Լ��������Ҳ�����Լ�����Ϊ
		// �Լ���������Լ������һ��
		// Ȼ��ģ�����һ�����ÿ�
 
		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
		myBlock.set(x, y, o).place();
 
		// �Ҹ��Է�ʲô�����ţ�
		typeCountForColor[0][enemyColor][blockType]++;
		nextTypeForColor[enemyColor] = blockType;
 
		// Ȼ����Լ������룬Ҳ���ǶԷ�����Ϊ
		// ���и��Լ��������ǶԷ������һ��
		scanf("%d%d%d%d", &blockType, &x, &y, &o);
 
		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
		enemyBlock.set(x, y, o).place();
 
		// �Է�����ʲô�����ţ�
		typeCountForColor[0][currBotColor][blockType]++;
		nextTypeForColor[currBotColor] = blockType;
 
		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);
 
		// ����ת��
		Util::transfer();
	}
	
	int blockForEnemy, finalX, finalY, finalO;
 
	// �������ߣ���ֻ���޸����²��֣�
	
	cur = currBotColor, ene = enemyColor; int cnt;
	
	for(cnt=1; (double)(clock()-startTime)/CLOCKS_PER_SEC < 0.5; cnt++)
	//for(cnt=1; cnt <= 421; cnt++)
	{
		actCnt = 1;
		nowGame = 0;
		curType = nextTypeForColor[currBotColor];
		eneType = nextTypeForColor[enemyColor];
		ED = 0;
		MCTS(0);
	} //printf("%d\n", cnt);
	v2type=1; v2ave=v2sum/v2cnt;
	for(cnt=1; (double)(clock()-startTime)/CLOCKS_PER_SEC < 1; cnt++)
	//for(cnt=1; cnt <= 550; cnt++)
	{
		actCnt = cnt;
		nowGame = 0;
		curType = nextTypeForColor[currBotColor];
		eneType = nextTypeForColor[enemyColor];
		ED = 0;
		MCTS(0);
	} //printf("%d\n", cnt);
	
	
	actCnt = MCTS_Cnt;
	double mx=-1e30; rep(i, ST[0], ST[1]-1) 
		g[i].v2 = g[i].v/(1+log2(actCnt)) + (g[i].w+g[i].l ? 1.0*g[i].w/(g[i].w+g[i].l) : 0.5),
		mx=max(mx, g[i].v2);
	int Ans=ST[0]; while (g[Ans].v2 != mx) Ans++;
	finalX=g[Ans].x, finalY=g[Ans].y, finalO=g[Ans].o;
	
	/* nowGame=0;
	Tetris block(nextTypeForColor[currBotColor], currBotColor);
	block.set(g[Ans].x, g[Ans].y, g[Ans].o).place();
	Util::printField(); */
	
	mx=-1e30; rep(i, ST2[0], ST2[1]-1) 
		g2[i].v2 = (g2[i].w+g2[i].l ? 1.0*g2[i].w/(g2[i].w+g2[i].l) : 0.5),
		mx=max(mx, g2[i].v2);
	Ans=ST2[0]; while (g2[Ans].v2 != mx) Ans++;
	blockForEnemy=g2[Ans].ty;
	
	// ���߽���������������ֻ���޸����ϲ��֣�
	
#ifndef _BOTZONE_ONLINE
	freopen("player0_action.txt", "w", stdout);
#endif

	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;
	
	return 0;
}