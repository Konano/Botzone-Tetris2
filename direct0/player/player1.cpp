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

#define MaxGame 10000

int ED, nowGame, gameCnt;

inline int bit(int x)
{
    x=(x & 0x55555555)+((x>>1) & 0x55555555);
    x=(x & 0x33333333)+((x>>2) & 0x33333333);
    x=(x & 0x0F0F0F0F)+((x>>4) & 0x0F0F0F0F);
    x=(x & 0x00FF00FF)+((x>>8) & 0x00FF00FF);
    x=(x & 0x0000FFFF)+((x>>16) & 0x0000FFFF);
    return x;
}

inline double rand01(){return (1.0*rand()/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1))+(1.0*rand()/(RAND_MAX+1)/(RAND_MAX+1)/(RAND_MAX+1));}

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
		blockX = x == -1 ? blockX : x;
		blockY = y == -1 ? blockY : y;
		orientation = o == -1 ? orientation : o;
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
		eliminateUnit = 0;
		int &count = transCount[color] = 0;
		maxHeight[color] = MAPHEIGHT;
		for (int i = 1; i <= MAPHEIGHT; i++)
		{
			if (gridInfo[nowGame][color][i] == Full)
			{
				eliminateUnit += block.inRow(i);
				
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
		
		eliminateRow = count;
		if (count == 0) elimCombo[nowGame][color] = 0;
		maxHeight[color] -= count - (elimCombo[nowGame][color]>=3);
		elimTotal[nowGame][color] += elimBonus[count - (elimCombo[nowGame][color]>=3)];
		eliminateSum[color] += eliminateRow * eliminateUnit;
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
		
		dow(y, 20, 1) if (grid[y]) {maxHeight = y; break;}
		rep(y, 1, 20) boardRowTransitions += bit(grid[y] ^ (grid[y]>>1))-1,
					  boardColTransitions += bit(grid[y] ^ grid[y-1]);
		int a;
		a=0; dow(y, 20, 1) boardBuriedHoles += bit(a&(~grid[y])),
						   boradHoleRow += (a&(~grid[y])?1:0), 
						   a|=grid[y];
		a=0; rep(y, 1, 20) boradHoleDepth += bit(a&grid[y]), a|=(~grid[y]);
		
		for(int x=1,W=0; x<=10; x++,W=0) dow(y, 20, 1)
			if (grid[y]&(1<<x)) W = 0; else 
				boardWells += (W += ((grid[y]&(1<<(x-1)))&&(grid[y]&(1<<(x+1)))));
	}
	
	inline void print()
	{
		printf("%d %d %d %d %d %d %d\n", erodedPieceCellsMetric, boardRowTransitions, boardColTransitions, boardBuriedHoles, boardWells, boradHoleDepth, boradHoleRow);
	}
}

inline double Value(Tetris block, int currBotColor)
{
	double value=0;
	value += -1.262900e+01 * block.landingHeight();
	
	Util::fork(); block.place(); Util::eliminate(currBotColor, block); Feature::GetFeature(currBotColor);
	
	value += +6.601974e+00 * Feature::erodedPieceCellsMetric;
	value += -9.215815e+00 * Feature::boardRowTransitions;
	value += -1.977356e+01 * Feature::boardColTransitions;
	value += -1.308335e+01 * Feature::boardBuriedHoles;
	value += -1.048747e+01 * Feature::boardWells;
	value += -1.611863e+00 * Feature::boradHoleDepth;
	value += -2.404087e+01 * Feature::boradHoleRow;
	
	/* if (block.blockX == 5 && block.blockY == 3 && block.orientation == 3)
		Feature::print();
	if (block.blockX == 5 && block.blockY == 14 && block.orientation == 1)
		Feature::print(); */
	
	nowGame--;
	return value;
}

int curactnum, eneactnum;
struct act{int x,y,o; double v,v2; int w,l;} g[109], h[109];
bool cmp(act a, act b){return a.v>b.v;}

struct act2{bool b; double v; int w,l;} k[MaxGame][7]; int NEXT[MaxGame][7], A[MaxGame];
int cur, ene;

bool vis[MAPWIDTH+2][MAPHEIGHT+4][4];
struct node{int x,y,o;}; queue<node>q;
inline void GetAllAction(int cur, int type, act *a, int &st, int c, int curGame)
{
	int maxCount = 0, minCount = 99;
	rep(ty, 0, 6)
	{
		if (typeCountForColor[nowGame][cur][ty] > maxCount)
			maxCount = typeCountForColor[nowGame][cur][ty];
		if (typeCountForColor[nowGame][cur][ty] < minCount)
			minCount = typeCountForColor[nowGame][cur][ty];
	}
	rep(ty, 0, 6) k[curGame][ty].b = (typeCountForColor[nowGame][cur][ty] != maxCount || maxCount - minCount != 2);
	
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
		
		if (block.onGround()) 
			a[st++] = (act){block.blockX, block.blockY, block.orientation, Value(block, cur), 0, 0, 0};
	}
	
	sort(a+tmp, a+st, cmp);
	int x=min(c,st-tmp); while (x!=st-tmp && a[tmp+x-1].v == a[tmp+x].v) x++;
	st=tmp+x;
	
	double mn=1e9, mx=-1e9; 
	rep(i, tmp, st-1) mn=min(mn,a[i].v), mx=max(mx,a[i].v);
	rep(i, tmp, st-1) a[i].v = (mx==mn ? 1 : (a[i].v-mn)/(mx-mn));
}

inline double Value2(int color)
{
	double v=0; Feature::GetFeature(color);
	v += -1.262900e+01 * Feature::maxHeight;
	v += +20.601974 * eliminateSum[color];
	v += -9.215815e+00 * Feature::boardRowTransitions;
	v += -1.977356e+01 * Feature::boardColTransitions;
	v += -1.308335e+01 * Feature::boardBuriedHoles;
	v += -1.048747e+01 * Feature::boardWells;
	v += -1.611863e+00 * Feature::boradHoleDepth;
	v += -2.404087e+01 * Feature::boradHoleRow;
	return v;
}

inline int Choose(act *a, int L, int R, int actCnt)
{
	double sum=0;
	rep(i, L, R) 
		sum += (a[i].v2 = a[i].v/(1+log2(max(actCnt,1))) + (a[i].w+a[i].l ? 1.0*a[i].w/(a[i].w+a[i].l) : 0.5) + sqrt(2*log(max(actCnt,1))/(a[i].w+a[i].l ? a[i].w+a[i].l : 0.5)));
	double tmp=rand01()*sum; 
	int x=L; while (tmp>a[x].v2) tmp-=a[x++].v2;
	return x;
}
inline int Choose(int curGame)
{
	double sum=0; rep(i, 0, 6) if (k[curGame][i].b)
		sum += (k[curGame][i].v = (k[curGame][i].w+k[curGame][i].l ? 1.0*k[curGame][i].w/(k[curGame][i].w+k[curGame][i].l) : 0.5) + sqrt(2*log(A[curGame]?A[curGame]:1)/(k[curGame][i].w+k[curGame][i].l ? k[curGame][i].w+k[curGame][i].l : 0.5)));
	double tmp=rand01()*sum; 
	int x=0; while (tmp>k[curGame][x].v) tmp-=k[curGame][x++].v;
	return x;
}
inline int Choose(int L, int R)
{
	double sum=0;
	rep(i, L, R) sum+=1.0/(i-L+1);
	double tmp=rand01()*sum; 
	int x=L; while (tmp>1.0/(x-L+1)) tmp-=1.0/(x-L+1), x++;
	return x;
}

vector<double>v2[2]; double v2ave[2]; int v2type;

void MCTS(int layer, int curGame, int eneGame, int curType, int eneType)
{
	int gST, gED, hST, hED;
	
	GetAllAction(cur, curType, g, gED=gST=curactnum, 3, curGame);
	if (gST==gED) ED|=(1<<cur);
	GetAllAction(ene, eneType, h, hED=hST=eneactnum, 3, eneGame);
	if (hST==hED) ED|=(1<<ene);
	if (ED==3) ED = elimTotal[nowGame][cur] > elimTotal[nowGame][ene];
	if (ED) return;
	
	int curAct = Choose(gST, gED);
	int curNext = Choose(curGame);
	int eneAct = Choose(hST, hED);
	int eneNext = Choose(eneGame);
	
	if (!NEXT[curGame][curNext]) NEXT[curGame][curNext] = ++gameCnt;
	if (!NEXT[eneGame][eneNext]) NEXT[eneGame][eneNext] = ++gameCnt;
	
	Tetris curBlock(curType, cur);
	curBlock.set(g[curAct].x, g[curAct].y, g[curAct].o).place();
	Tetris eneBlock(eneType, ene);
	eneBlock.set(h[eneAct].x, h[eneAct].y, h[eneAct].o).place();
	typeCountForColor[nowGame][ene][eneNext]++;
	typeCountForColor[nowGame][cur][curNext]++;
	Util::eliminate(cur, curBlock);
	Util::eliminate(ene, eneBlock);
	Util::transfer();
	
	if (layer == 1)
	{
		if (!v2type)
		{
			if (ED==(1<<cur)) v2[cur].pb(-5000); else v2[cur].pb(Value2(cur));
			if (ED==(1<<ene)) v2[ene].pb(-5000); else v2[ene].pb(Value2(ene));
			return;
		}
		if (!ED)
		{
			if (Value2(cur)<v2ave[cur]) ED|=(1<<cur);
			if (Value2(ene)<v2ave[ene]) ED|=(1<<ene);
			return;
		}
	}
	
	if (ED && v2type) A[curGame]++, A[eneGame]++;
	if (ED) return;
	
	MCTS(layer+1, NEXT[curGame][curNext], NEXT[eneGame][eneNext], curNext, eneNext);
	
	if (v2type)
	{
		if (ED & (1<<cur)) k[curGame][curNext].w++; else k[curGame][curNext].l++;
		if (ED & (1<<ene)) k[eneGame][eneNext].w++; else k[eneGame][eneNext].l++;
		A[curGame]++, A[eneGame]++;
	}
}

inline void MCTS0(int curType, int eneType)
{
	Util::fork(); ED = 0; eliminateSum[0] = eliminateSum[1] = 0;
	
	int curAct = Choose(g, 0, curactnum-1, A[1]);
	int curNext = Choose(1);
	int eneAct = Choose(h, 0, eneactnum-1, A[1]);
	int eneNext = Choose(2);
	
	if (!NEXT[1][curNext]) NEXT[1][curNext] = ++gameCnt;
	if (!NEXT[2][eneNext]) NEXT[2][eneNext] = ++gameCnt;
	
	Tetris curBlock(curType, cur);
	curBlock.set(g[curAct].x, g[curAct].y, g[curAct].o).place();
	Tetris eneBlock(eneType, ene);
	eneBlock.set(h[eneAct].x, h[eneAct].y, h[eneAct].o).place();
	typeCountForColor[nowGame][ene][eneNext]++;
	typeCountForColor[nowGame][cur][curNext]++;
	Util::eliminate(cur, curBlock);
	Util::eliminate(ene, eneBlock);
	Util::transfer();
	
	if (ED && v2type)
	{
		if (ED & (1<<cur)) g[curAct].l++; else g[curAct].w++;
		if (ED & (1<<ene)) h[eneAct].l++; else h[eneAct].w++;
	}
	if (ED) {nowGame--; return;}
	
	MCTS(1, NEXT[1][curNext], NEXT[2][eneNext], curNext, eneNext);
	
	if (v2type)
	{
		if (ED & (1<<cur)) k[1][curNext].w++, g[curAct].l++; else k[1][curNext].l++, g[curAct].w++;
		if (ED & (1<<ene)) k[2][eneNext].w++, h[eneAct].l++; else k[2][eneNext].l++, h[eneAct].w++;
		A[1]++, A[2]++;
	}
	
	nowGame--;
}

int main()
{
	clock_t startTime = clock();
	
	// 加速输入
	istream::sync_with_stdio(false);
	//srand(time(NULL));
	srand(2333);
	Util::init();
	
#ifndef _BOTZONE_ONLINE
	freopen("input1.txt", "r", stdin);
#endif
 
	int blockType, enemyColor, currBotColor;
	int nextTypeForColor[2];
 
	// 先读入第一回合，得到自己的颜色
	// 双方的第一块肯定是一样的
	scanf("%d%d", &blockType, &currBotColor);
	enemyColor = 1 - currBotColor;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][0][blockType]++;
	typeCountForColor[0][1][blockType]++;
 
	// 然后分析以前每回合的输入输出，并恢复状态
	// 循环中，color 表示当前这一行是 color 的行为
	// 平台保证所有输入都是合法输入
	int x, y, o; while (scanf("%d%d%d%d", &blockType, &x, &y, &o)!=EOF)
	{
		int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
		// 根据这些输入输出逐渐恢复状态到当前回合
 
		// 先读自己的输出，也就是自己的行为
		// 自己的输出是自己的最后一步
		// 然后模拟最后一步放置块
 
		// 我当时把上一块落到了 x y o！
		Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
		myBlock.set(x, y, o).place();
 
		// 我给对方什么块来着？
		typeCountForColor[0][enemyColor][blockType]++;
		nextTypeForColor[enemyColor] = blockType;
 
		// 然后读自己的输入，也就是对方的行为
		// 裁判给自己的输入是对方的最后一步
		scanf("%d%d%d%d", &blockType, &x, &y, &o);
 
		// 对方当时把上一块落到了 x y o！
		Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
		enemyBlock.set(x, y, o).place();
 
		// 对方给我什么块来着？
		typeCountForColor[0][currBotColor][blockType]++;
		nextTypeForColor[currBotColor] = blockType;
		
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
 
	// 做出决策（你只需修改以下部分）
	
	cur = currBotColor, ene = enemyColor;
	GetAllAction(cur, nextTypeForColor[currBotColor], g, curactnum=0, 5, 1);
	GetAllAction(ene, nextTypeForColor[enemyColor], h, eneactnum=0, 5, 2);
	gameCnt = 2;
	
	int cnt; 
	for(cnt=0; (double)(clock()-startTime)/CLOCKS_PER_SEC < 0.3; cnt++) MCTS0(nextTypeForColor[currBotColor], nextTypeForColor[enemyColor]);
	//for(cnt=0; cnt<=300; cnt++) MCTS0(nextTypeForColor[currBotColor], nextTypeForColor[enemyColor]);
	
	v2type=1; 
	if (!v2[0].empty()) sort(v2[0].begin(), v2[0].end()), v2ave[0]=v2[0][(int)((double)v2[0].size()*0.7)];
	if (!v2[1].empty()) sort(v2[1].begin(), v2[1].end()), v2ave[1]=v2[1][(int)((double)v2[1].size()*0.7)];
	
	for(cnt=0; cnt<=300; cnt++) MCTS0(nextTypeForColor[currBotColor], nextTypeForColor[enemyColor]);
	//for(cnt=0; (double)(clock()-startTime)/CLOCKS_PER_SEC < 1.0; cnt++) MCTS0(nextTypeForColor[currBotColor], nextTypeForColor[enemyColor]);
	
	double mx=-1e30; rep(i, 0, curactnum-1) 
		g[i].v2 = g[i].v/(1+log2(cnt)) + (g[i].w+g[i].l ? 1.0*g[i].w/(g[i].w+g[i].l) : 0.5),
		mx=max(mx, g[i].v2);
	int Ans=0; while (g[Ans].v2 != mx) Ans++;
	finalX=g[Ans].x, finalY=g[Ans].y, finalO=g[Ans].o;
	
	/* Tetris block(nextTypeForColor[currBotColor], currBotColor);
	block.set(g[Ans].x, g[Ans].y, g[Ans].o).place();
	Util::printField(); */
	
	mx=-1e30; rep(i, 0, 6) if (k[2][i].b)
		k[2][i].v = (k[2][i].w+k[2][i].l ? 1.0*k[2][i].w/(k[2][i].w+k[2][i].l) : 0.5),
		mx=max(mx, k[2][i].v);
	Ans=0; while (!k[2][Ans].b || k[2][Ans].v!=mx) Ans++;
	blockForEnemy=Ans;
	
	// 决策结束，输出结果（你只需修改以上部分）
	
#ifndef _BOTZONE_ONLINE
	//freopen("player1_action.txt", "w", stdout);
#endif

	cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;
	
	return 0;
}