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
#define lowbit(x) (x&-x)
#define fi first
#define se second

#define grid(c,x,y) ((gridInfo[nowGame][c][y]>>(x))&1)
#define setgrid(c,x,y) (gridInfo[nowGame][c][y]|=(1<<(x)))
#define setgrid0(c,x,y) (gridInfo[nowGame][c][y]-=(1<<(x)))

typedef long long ll;
typedef pair<int,int> Pii;
typedef pair<ll,int> Pli;
typedef pair<double,int> Pdi;
typedef long double ld;
typedef unsigned long long ull;
typedef pair<pair<int,int>,pair<int,int> > PPP;

// 注意：x的范围是1~MAPWIDTH，y的范围是1~MAPHEIGHT
// 数组是先行（y）后列（c）
// 坐标系：原点在左下角

#define MAPWIDTH 10
#define MAPHEIGHT 20

#define MaxGame 50000

int ED, nowGame, gameCnt, roundCnt, SEARCH; double EXPOR;

inline int bit(int x)
{
    x=(x & 0x55555555)+((x>>1) & 0x55555555);
    x=(x & 0x33333333)+((x>>2) & 0x33333333);
    x=(x & 0x0F0F0F0F)+((x>>4) & 0x0F0F0F0F);
    x=(x & 0x00FF00FF)+((x>>8) & 0x00FF00FF);
    x=(x & 0x0000FFFF)+((x>>16) & 0x0000FFFF);
    return x;
}

#ifndef _BOTZONE_ONLINE
inline double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}
#endif

#ifdef _BOTZONE_ONLINE
inline double rand01(){return 1.0*rand()/RAND_MAX;}
#endif

// 先y后x，记录地图状态，0为空，1为以前放置，2为刚刚放置，负数为越界
// （2用于在清行后将最后一步撤销再送给对方）
int gridInfo[3][2][MAPHEIGHT+2] = { 0 };

// 代表分别向对方转移的行
int trans[2][6] = { 0 };

// 转移行数
int transCount[2] = { 0 };

// 运行eliminate后的当前高度
int maxHeight[2] = { 0 };

// 总消去行数的分数之和
int elimTotal[3][2] = { 0 };

// 连续几回合发生过消去了
int elimCombo[3][2] = { 0 };

// 给对应玩家的各类块的数目总计
int typeCountForColor[3][2][7] = { 0 };

// 下一个方块
int nextTypeForColor[3][2];

// 一次性消去行数对应分数
const int elimBonus[] = { 0, 1, 3, 5, 7 };

const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
}; // 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

const int rotateBlank[7][4][10] = {
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { -1,-1,-1,1,1,1,0,0 },{ -1,-1,-1,1,1,-1,0,0 },{ -1,-1,1,1,1,-1,0,0 },{ -1,1,1,1,1,-1,0,0 } },
	{ { 1,-1,-1,1,-2,1,-1,2,-2,2 } ,{ 1,1,-1,-1,-2,-1,-1,-2,-2,-2 } ,{ -1,1,1,-1,2,-1,1,-2,2,-2 } ,{ -1,-1,1,1,2,1,1,2,2,2 } },
	{ { 0,0 },{ 0,0 } ,{ 0,0 } ,{ 0,0 } }
}; // 旋转的时候需要为空的块相对于旋转中心的坐标

class Tetris
{
public:
	const int blockType;   // 标记方块类型的序号 0~6
	int blockX;            // 旋转中心的x轴坐标
	int blockY;            // 旋转中心的y轴坐标
	int orientation;       // 标记方块的朝向 0~3
	const int(*shape)[8]; // 当前类型方块的形状定义
	int color;
 
	Tetris(int t, int color) : blockType(t), shape(blockShape[t]), color(color)
	{ }
 
	inline Tetris &set(int x = -1, int y = -1, int o = -1)
	{
		blockX = (x == -1 ? blockX : x);
		blockY = (y == -1 ? blockY : y);
		orientation = (o == -1 ? orientation : o);
		return *this;
	}
 
	// 判断当前位置是否合法
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
				grid(color, tmpX, tmpY))
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
			if (grid(color, tmpX, tmpY)) return false;
		}
		return true;
	}
	
	inline bool onTop()
	{
		if (!isValid2()) return false;
		rep(i, 0, 3) if (blockY + shape[orientation][i*2+1] < MAPHEIGHT) return false;
		return true;
	}
 
	// 判断是否落地
	inline bool onGround()
	{
		if (isValid() && !isValid(-1, blockY - 1))
			return true;
		return false;
	}
 
	// 将方块放置在场地上
	inline bool place()
	{
		if (!onGround())
			return false;
		
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = blockX + shape[orientation][2 * i];
			tmpY = blockY + shape[orientation][2 * i + 1];
			setgrid(color, tmpX, tmpY);
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
			setgrid0(color, tmpX, tmpY);
		}
	}
	
	inline int inRow(int y)
	{
		int i, cnt=0;
		for (i = 0; i < 4; i++) cnt+=(blockY + shape[orientation][2 * i + 1] == y);
		return cnt;
	}
	
	// 检查能否向下移动
	inline bool movedown(){return isValid2(-1, blockY - 1);}
	inline bool moveleft(){return isValid(blockX - 1);}
	inline bool moveright(){return isValid(blockX + 1);}
 
	// 检查能否逆时针旋转自己到o
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
	            
	        // 检查旋转碰撞
	        for (i = 0; i < 5; i++) {
	            blankX = blockX + rotateBlank[blockType][fromO][2 * i];
	            blankY = blockY + rotateBlank[blockType][fromO][2 * i + 1];
	            if (blankX == blockX && blankY == blockY)
	                break;
	            if (grid(color, blankX, blankY))
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

int eliminateUnit, eliminateRow, eliminateSum[2];

namespace Util
{
	// 围一圈护城河
	inline void init()
	{
		int i;
		for (i = 0; i < MAPHEIGHT + 2; i++)
		{
			setgrid(0, 0, i);
			setgrid(0, MAPWIDTH + 1, i);
			setgrid(1, 0, i);
			setgrid(1, MAPWIDTH + 1, i);
		}
		for (i = 0; i < MAPWIDTH + 2; i++)
		{
			setgrid(0, i, 0);
			setgrid(0, i, MAPHEIGHT + 1);
			setgrid(1, i, 0);
			setgrid(1, i, MAPHEIGHT + 1);
		}
	}
	
#define Wall ((1<<0)+(1<<11))
#define Full ((1<<12)-1)
	
	// 消去行
	void eliminate(int color, Tetris &block)
	{
		eliminateUnit = 2;
		int &count = transCount[color] = 0;
		maxHeight[color] = MAPHEIGHT;
		for (int i = 1; i <= MAPHEIGHT; i++)
		{
			if (gridInfo[nowGame][color][i] == Full)
			{
				block.remove();
				if (!count && ++elimCombo[nowGame][color] >= 3)
					trans[color][count++] = gridInfo[nowGame][color][i]; // 奖励行
				trans[color][count++] = gridInfo[nowGame][color][i];
				block.place();
			}
			else if (gridInfo[nowGame][color][i] == Wall)
			{
				maxHeight[color] = i-1;
				break;
			}
		}
		
		for (int i = 1, cnt = 0; i <= MAPHEIGHT; i++)
		{
			if (gridInfo[nowGame][color][i] == Full) cnt++, gridInfo[nowGame][color][i]=Wall;
			else if (cnt) gridInfo[nowGame][color][i-cnt]=gridInfo[nowGame][color][i], gridInfo[nowGame][color][i]=Wall;
		}
		
		eliminateRow = count - (elimCombo[nowGame][color]>=3) + max(0,elimCombo[nowGame][color]-1);
		eliminateSum[color] += eliminateUnit * eliminateRow;
		if (count == 0) elimCombo[nowGame][color] = 0;
		maxHeight[color] -= count - (elimCombo[nowGame][color]>=3);
		elimTotal[nowGame][color] += elimBonus[count - (elimCombo[nowGame][color]>=3)];
	}
 
	// 转移双方消去的行，返回-1表示继续，否则返回输者
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
			int i;
 
			for (i = h2; i > transCount[color1]; i--)
				gridInfo[nowGame][color2][i] = gridInfo[nowGame][color2][i - transCount[color1]];
 
			for (i = transCount[color1]; i > 0; i--)
				gridInfo[nowGame][color2][i] = trans[color1][i - 1];
			return -1;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//从color1处移动count1去color2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
 
			if (h1 > MAPHEIGHT) return ED=(1<<color1),color1;
			if (h2 > MAPHEIGHT) return ED=(1<<color2),color2;
 
			int i;
			for (i = h2; i > transCount[color1]; i--)
				gridInfo[nowGame][color2][i] = gridInfo[nowGame][color2][i - transCount[color1]];
 
			for (i = transCount[color1]; i > 0; i--)
				gridInfo[nowGame][color2][i] = trans[color1][i - 1];
 
			for (i = h1; i > transCount[color2]; i--)
				gridInfo[nowGame][color1][i] = gridInfo[nowGame][color1][i - transCount[color2]];
 
			for (i = transCount[color2]; i > 0; i--)
				gridInfo[nowGame][color1][i] = trans[color2][i - 1];
 
			return -1;
		}
	}
	
	inline void fork()
	{
		rep(o, 0, 1) rep(i, 0, MAPHEIGHT+1)
			gridInfo[nowGame+1][o][i] = gridInfo[nowGame][o][i];
		
		rep(o, 0, 1)
			elimTotal[nowGame+1][o] = elimTotal[nowGame][o],
			elimCombo[nowGame+1][o] = elimCombo[nowGame][o];
		
		rep(o, 0, 1) rep(i, 0, 6)
			typeCountForColor[nowGame+1][o][i] = typeCountForColor[nowGame][o][i];
		
		rep(o, 0, 1) nextTypeForColor[nowGame+1][o] = nextTypeForColor[nowGame][o];
		
		++nowGame;
	}
	
	// 打印场地用于调试
	inline void printField()
	{
#ifndef _BOTZONE_ONLINE
		static const char *i2s[] = {
			"  ",
			"[]"
		};
		for (int y = MAPHEIGHT + 1; y >= 0; y--)
		{
			for (int x = 0; x <= MAPWIDTH + 1; x++) cout << i2s[grid(0,x,y)];
			cout << "||";
			for (int x = 0; x <= MAPWIDTH + 1; x++) cout << i2s[grid(1,x,y)];
			cout << endl;
		}
		cout << endl;
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
		const int(*grid) = gridInfo[nowGame][color];
		maxHeight = boardRowTransitions = boardColTransitions = boardBuriedHoles = boardWells = boradHoleDepth = boradHoleRow = 0;
		
		erodedPieceCellsMetric = eliminateUnit*eliminateRow;
		
		dow(y, 20, 1) if (grid[y] != Wall) {maxHeight = y; break;}
		rep(y, 1, 20) boardRowTransitions += bit(grid[y] ^ (grid[y]>>1))-1,
					  boardColTransitions += bit(grid[y] ^ grid[y-1]);
		int a;
		a=0; dow(y, 20, 1) boardBuriedHoles += bit(a&(~grid[y])),
						   boradHoleRow += (a&(~grid[y])?1:0), 
						   a|=grid[y];
		a=0; rep(y, 1, 20) boradHoleDepth += bit(a&grid[y]), a|=(~grid[y]);
		
		for(int x=1,B=0; x<=10; x++,B=0) rep(y, 1, 20)
		{
			B = (grid[y]&(1<<x) ? 0 : B+1);
			if ((grid[y]&(1<<(x-1))) && (grid[y]&(1<<(x+1)))) 
				boardWells += B;
		}
	}
}

inline double Value(Tetris block, int currBotColor)
{
	double value=0; int tmp = eliminateSum[currBotColor];
	value += -1.262900e+01 * block.landingHeight();
	
	Util::fork(); block.place(); Util::eliminate(currBotColor, block); Feature::GetFeature(currBotColor);
	
	value += +6.601974e+00 * Feature::erodedPieceCellsMetric;
	value += -9.215815e+00 * Feature::boardRowTransitions;
	value += -1.977356e+01 * Feature::boardColTransitions;
	value += -1.308335e+01 * Feature::boardBuriedHoles;
	value += -1.048747e+01 * Feature::boardWells;
	value += -1.611863e+00 * Feature::boradHoleDepth;
	value += -2.404087e+01 * Feature::boradHoleRow;
	
	nowGame--; eliminateSum[currBotColor] = tmp;
	return value;
}

int curactnum, eneactnum;
struct act{int x,y,o; double v,v2,w,l;} g[4*MaxGame], a[109];
bool cmp(act a, act b){return a.v>b.v;}

struct act2{bool b; double v,w,l;} k[MaxGame][7]; int ST[MaxGame], A[MaxGame];
map<ull,int>M;
int cur, ene;

bool vis[MAPWIDTH+2][MAPHEIGHT+4][4];
struct node{int x,y,o;}; queue<node>q;

inline void MakeNewGame(int c, int game, ull H)
{
	M[H]=game;
	
	int maxCount = 0, minCount = 99;
	rep(ty, 0, 6)
	{
		if (typeCountForColor[nowGame][c][ty] > maxCount)
			maxCount = typeCountForColor[nowGame][c][ty];
		if (typeCountForColor[nowGame][c][ty] < minCount)
			minCount = typeCountForColor[nowGame][c][ty];
	}
	rep(ty, 0, 6) k[game][ty].b = (typeCountForColor[nowGame][c][ty] != maxCount || maxCount - minCount != 2);
	
	int cnt = 0, type = nextTypeForColor[nowGame][c]; Tetris block(type, c); clr(vis,0);
	
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
		
		if (block.onGround()) 
			a[cnt++] = (act){block.blockX, block.blockY, block.orientation, Value(block, c), 0, 0, 0};
	}
	
	if (cnt) sort(a, a+cnt, cmp);
	int x=min((game<=2?5:3),cnt); while (x<cnt && a[x-1].v == a[x].v) x++;
	
	double mn=1e9, mx=-1e9; 
	rep(i, 0, x-1) mn=min(mn,a[i].v), mx=max(mx,a[i].v);
	rep(i, 0, x-1) a[i].v = (mx==mn ? 1 : (a[i].v-mn)/(mx-mn));
	
	ST[game+1] = ST[game]+x;
	rep(i, 0, x-1) g[ST[game]+i] = a[i];
}

inline double Value2(int color)
{
	double v=0; Feature::GetFeature(color);
	v += -1.262900e+01 * Feature::maxHeight;
	v += +6.601974e+00 * eliminateSum[color];
	v += -9.215815e+00 * Feature::boardRowTransitions;
	v += -1.977356e+01 * Feature::boardColTransitions;
	v += -1.308335e+01 * Feature::boardBuriedHoles;
	v += -1.048747e+01 * Feature::boardWells;
	v += -1.611863e+00 * Feature::boradHoleDepth;
	v += -2.404087e+01 * Feature::boradHoleRow;
	
	return v;
}

inline ull GetHash(int color)
{
	ull X=0;
	rep(y, 1, 20) X = X*233 + gridInfo[nowGame][color][y];
	
	int minCount = 99, tmp = nextTypeForColor[nowGame][color];
	rep(ty, 0, 6) if (typeCountForColor[nowGame][color][ty] < minCount) minCount = typeCountForColor[nowGame][color][ty];
	rep(ty, 0, 6) tmp=tmp*3+typeCountForColor[nowGame][color][ty]-minCount;
	X = X*233 + tmp;
	
	return X;
}

Pdi q0[10];
inline double P(double a, int t){double x=1; while (t) {if (t&1) x=x*a; t>>=1, a=a*a;} return x;}

inline int Act(int game, int layer)
{
	if (layer==0)
	{
		rep(i, ST[game], ST[game+1]-1)
		{
			if (g[i].w+g[i].l)
				g[i].v2 = g[i].v/(1+log2(A[game])) + g[i].w/(g[i].w+g[i].l);
			else
				g[i].v2 = g[i].v + 0.5;
			q0[i-ST[game]] = Pdi(-g[i].v2,i);
		}
		sort(q0, q0+ST[game+1]-ST[game]);
		double sum=0; 
		rep(i, 0, ST[game+1]-ST[game]-1) sum += (q0[i].fi=P(1-0.0006*i,roundCnt));
		double tmp=rand01()*sum;
		rep(i, 0, ST[game+1]-ST[game]-1) if (tmp>q0[i].fi) tmp-=q0[i].first; else return q0[i].se;
	}
	else
	{
		double sum=0; rep(i, ST[game], ST[game+1]-1)
		{
			g[i].v2 = g[i].v/(1+log2(A[game]?A[game]:1)) + (g[i].w+1)/(g[i].w+g[i].l+2);
			sum += g[i].v2;
		}
		double tmp=rand01()*sum;
		rep(i, ST[game], ST[game+1]-1) if (tmp>g[i].v2) tmp-=g[i].v2; else return i;
	}
	return 114514;
}

inline int Choose(int game, int layer)
{
	if (layer==0)
	{
		int tot=0; rep(i, 0, 6) if (k[game][i].b)
		{
			if (k[game][i].w+k[game][i].l)
				k[game][i].v = k[game][i].w/(k[game][i].w+k[game][i].l);
			else
				k[game][i].v = 0.5;
			q0[tot++] = Pdi(-k[game][i].v,i);
		}
		sort(q0, q0+tot);
		double sum=0; 
		rep(i, 0, tot-1) sum += (q0[i].first=P(1-0.0006*i,roundCnt));
		double tmp=rand01()*sum;
		rep(i, 0, tot-1) if (tmp>q0[i].first) tmp-=q0[i].first; else return q0[i].se;
	}
	else
	{
		double sum=0; rep(i, 0, 6) if (k[game][i].b)
		{
			k[game][i].v = (k[game][i].w+1)/(k[game][i].w+k[game][i].l+2);
			sum += k[game][i].v;
		}
		double tmp=rand01()*sum;
		rep(i, 0, 6) if (k[game][i].b) {if (tmp>k[game][i].v) tmp-=k[game][i].v; else return i;}
	}
	return 114514;
}

priority_queue<double, vector<double>, greater<double> >v2[2]; int upCnt;

#define MAXL 5

int db;
int Log[5][4];

void MCTS(int layer)
{
	ull htmp;
	
	int curGame = M[htmp = GetHash(cur)]; if (!curGame) MakeNewGame(cur, curGame=++gameCnt, htmp);
	int eneGame = M[htmp = GetHash(ene)]; if (!eneGame) MakeNewGame(ene, eneGame=++gameCnt, htmp);
	
	ED |= ((ST[curGame]==ST[curGame+1])<<cur) | ((ST[eneGame]==ST[eneGame+1])<<ene);
	if (ED == 3) ED = ((elimTotal[nowGame][cur] > elimTotal[nowGame][ene]) ? (1<<ene) : (1<<cur));
	
	int curAct=-1, curNext=-1, eneAct=-1, eneNext=-1;
	if (!ED)
	{
		Log[layer][0] = curAct = Act(curGame, layer);
		Log[layer][1] = curNext = Choose(curGame, layer);
		Log[layer][2] = eneAct = Act(eneGame, layer);
		Log[layer][3] = eneNext = Choose(eneGame, layer);
		
		A[curGame]++; A[eneGame]++;
		
		Tetris curBlock(nextTypeForColor[nowGame][cur], cur);
		curBlock.set(g[curAct].x, g[curAct].y, g[curAct].o).place();
		Tetris eneBlock(nextTypeForColor[nowGame][ene], ene);
		eneBlock.set(g[eneAct].x, g[eneAct].y, g[eneAct].o).place();
		typeCountForColor[nowGame][ene][eneNext]++;
		nextTypeForColor[nowGame][cur] = curNext;
		typeCountForColor[nowGame][cur][curNext]++;
		nextTypeForColor[nowGame][ene] = eneNext;
		Util::eliminate(cur, curBlock);
		Util::eliminate(ene, eneBlock);
		Util::transfer();
	}
	
	if (ED || layer == MAXL)
	{
		double tmpcur = ((ED&(1<<cur))==0 ? Value2(cur) : -100000);
		double tmpene = ((ED&(1<<ene))==0 ? Value2(ene) : -100000);
		if (tmpcur-tmpene < v2[cur].top()) ED|=(1<<cur); 
		if (tmpene-tmpcur < v2[ene].top()) ED|=(1<<ene);
		if (upCnt < 100.0/(0.5-EXPOR*0.48))
		{
			upCnt++;
			if ((ED&(1<<cur))==0 && tmpcur-tmpene<10000) v2[cur].pop(), v2[cur].push(tmpcur-tmpene);
			if ((ED&(1<<ene))==0 && tmpene-tmpcur<10000) v2[ene].pop(), v2[ene].push(tmpene-tmpcur);
			//printf("%d %.3lf %.3lf\n", upCnt, v2[cur].top(), v2[ene].top());
		}
	}
	else MCTS(layer+1);
	
	if (ED == 0 || ED == 3)
	{
		if (curNext!=-1) A[curGame]--, A[eneGame]--;
	}
	else if (ED == (1<<cur))
	{
		if (curAct!=-1) g[curAct].l+=1;
		if (curNext!=-1) k[curGame][curNext].w+=1;
		if (eneAct!=-1) g[eneAct].w+=1;
		if (eneNext!=-1) k[eneGame][eneNext].l+=1;
	}
	else if (ED == (1<<ene))
	{
		if (curAct!=-1) g[curAct].w+=1;
		if (curNext!=-1) k[curGame][curNext].l+=1;
		if (eneAct!=-1) g[eneAct].l+=1;
		if (eneNext!=-1) k[eneGame][eneNext].w+=1;
	}
}



int main()
{
	clock_t startTime = clock();
	
	// 加速输入
	istream::sync_with_stdio(false);
	srand(2333);
	
#ifndef _BOTZONE_ONLINE
	freopen("input1.txt", "r", stdin);
#endif
	
	Util::init();
	int turnID, blockType, enemyColor, currBotColor;
 
	// 先读入第一回合，得到自己的颜色
	// 双方的第一块肯定是一样的
	scanf("%d%d", &blockType, &currBotColor); currBotColor = 0;
	enemyColor = 1 - currBotColor;
	nextTypeForColor[0][0] = blockType;
	nextTypeForColor[0][1] = blockType;
	typeCountForColor[0][0][blockType]++;
	typeCountForColor[0][1][blockType]++;
 
	// 然后分析以前每回合的输入输出，并恢复状态
	// 循环中，color 表示当前这一行是 color 的行为
	// 平台保证所有输入都是合法输入
	int x, y, o; while (scanf("%d%d%d%d", &blockType, &x, &y, &o)!=EOF)
	{
		int currTypeForColor[2] = { nextTypeForColor[0][0], nextTypeForColor[0][1] };
		
		// 根据这些输入输出逐渐恢复状态到当前回合
 
		// 先读自己的输出，也就是自己的行为
		// 自己的输出是自己的最后一步
		// 然后模拟最后一步放置块
 
		// 我当时把上一块落到了 x y o！
		Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
		myBlock.set(x, y, o).place();
 
		// 我给对方什么块来着？
		typeCountForColor[0][enemyColor][blockType]++;
		nextTypeForColor[0][enemyColor] = blockType;
 
		// 然后读自己的输入，也就是对方的行为
		// 裁判给自己的输入是对方的最后一步
		scanf("%d%d%d%d", &blockType, &x, &y, &o);
 
		// 对方当时把上一块落到了 x y o！
		Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
		enemyBlock.set(x, y, o).place();
 
		// 对方给我什么块来着？
		typeCountForColor[0][currBotColor][blockType]++;
		nextTypeForColor[0][currBotColor] = blockType;
		
		//Util::printField();
		
		// 检查消去
		Util::eliminate(currBotColor, myBlock);
		Util::eliminate(enemyColor, enemyBlock);
		
		//Util::printField();
		
		// 进行转移
		Util::transfer();
		
		//Util::printField();
	}
	
	int blockForEnemy, finalX, finalY, finalO;
	cur = currBotColor, ene = enemyColor;
	
	// 做出决策（你只需修改以下部分）
	
	rep(i, 1, 100) v2[0].push(-10000), v2[1].push(-10000); SEARCH = 10000;
	//roundCnt=0; while(roundCnt<SEARCH)
	roundCnt=0; while((double)(clock()-startTime)/CLOCKS_PER_SEC < 0.99)
	{
		Util::fork();
		ED=0; eliminateSum[0] = eliminateSum[1] = 0; EXPOR = min(1.0,(double)(clock()-startTime)/CLOCKS_PER_SEC/0.8);
		MCTS(1);
		if (ED!=0 && ED!=3)
			roundCnt++;
		//if (ED!=0 && ED!=3) roundCnt++,printf("\n"); else printf("*");
		//printf("%.3lf %.3lf\n", v2[0].top(), v2[1].top());
		nowGame--;
	}
	
	int game=M[GetHash(cur)]; double mx=-1e30; rep(i, ST[game], ST[game+1]-1)
	{
		g[i].v2 = g[i].v/(1+log2(max(A[game]/2,1))) + g[i].w/(g[i].w+g[i].l);
		mx=max(mx, g[i].v2);
	}
	rep(i, ST[game], ST[game+1]-1) if (g[i].v2 == mx)
	{
		finalX=g[i].x, finalY=g[i].y, finalO=g[i].o;
		break;
	}
	
	game=M[GetHash(ene)]; mx=-1e30;
	rep(i, 0, 6) if (k[game][i].b) mx=max(mx, k[game][i].w/(k[game][i].w+k[game][i].l)); //printf("%d %.3lf\n", i, k[game][i].w/(k[game][i].w+k[game][i].l));
	rep(i, 0, 6) if (k[game][i].b && k[game][i].w/(k[game][i].w+k[game][i].l) == mx)
	{
		blockForEnemy = i;
		break;
	}
	
	// 决策结束，输出结果（你只需修改以上部分）
	
#ifndef _BOTZONE_ONLINE
	freopen("player1_action.txt", "w", stdout);
#endif

	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO << endl << (double)(clock()-startTime)/CLOCKS_PER_SEC << ' ' << gameCnt << ' ' << roundCnt;
	
	return 0;
}