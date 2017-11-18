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

int ED;

// ע�⣺x�ķ�Χ��1~MAPWIDTH��y�ķ�Χ��1~MAPHEIGHT
// ���������У�y�����У�c��
// ����ϵ��ԭ�������½�
 
#define MAPWIDTH 10
#define MAPHEIGHT 20
 
// ��y��x����¼��ͼ״̬��0Ϊ�գ�1Ϊ��ǰ���ã�2Ϊ�ոշ��ã�����ΪԽ��
// ��2���������к����һ���������͸��Է���
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2];  // backup
 
// ����ֱ���Է�ת�Ƶ���
int trans[2][6][MAPWIDTH + 2];  // backup
 
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
	
	inline double landingHeight()
	{
		double sum=0;
		rep(i, 0, 3) sum += blockY + shape[orientation][2 * i + 1] - 1;
		return sum/4;
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
				if (gridInfo[color][i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				eliminateRow++;
				
				for (j = 1; j <= MAPWIDTH; j++)
					eliminateUnit += (gridInfo[color][i][j]>>1);
				
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
		elimTotal[color] += elimBonus[count - hasBonus];
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

namespace Feature
{
	inline int erodedPieceCellsMetric(){return eliminateUnit*eliminateRow;}
	inline int boardRowTransitions(int color)
	{
		int cnt=0;
		rep(x, 1, 10) rep(y, 1, 20) if (!gridInfo[color][y][x]) 
			cnt += (x!=1&&gridInfo[color][y][x-1]?1:0) + (x!=10&&gridInfo[color][y][x+1]?1:0);
		return cnt;
	}
	inline int boardColTransitions(int color)
	{
		int cnt=0;
		rep(x, 1, 10) rep(y, 1, 20) if (!gridInfo[color][y][x]) 
			cnt += (gridInfo[color][y-1][x]?1:0) + (y!=20&&gridInfo[color][y+1][x]?1:0);
		return cnt;
	}
	inline int boardBuriedHoles(int color)
	{
		int cnt=0; bool fg;
		rep(x, 1, 10)
		{
			fg=false;
			dow(y, 20, 1) if (gridInfo[color][y][x]) fg=true; else cnt+=fg;
		}
		return cnt;
	}
	inline int boardWells(int color)
	{
		int cnt=0, L=0;
		rep(x, 1, 10) dow(y, 20, 0)
		{
			if (!gridInfo[color][y][x] && gridInfo[color][y][x-1] && gridInfo[color][y][x+1]) 
				L++; 
			else if (L) 
				cnt+=L*(L+1)/2, L=0;
		}
		return cnt;
	}
}

#define NerN 5

struct Neuron{double w[NerN];} Ner[2];

inline void GetNer(int a)
{
	char filename[12]="player0.txt";
	filename[6]='0'+a;
	freopen(filename, "r", stdin);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[a].w[i]);
	fclose(stdin);
}
inline double Value(Tetris block, int currBotColor, int NerColor)
{
	double value=0;
	value += -1 * block.landingHeight();
	
	Util::backup(); block.place(); Util::eliminate(currBotColor);
	
	value += Ner[NerColor].w[0] * Feature::erodedPieceCellsMetric();
	value += Ner[NerColor].w[1] * Feature::boardRowTransitions(currBotColor);
	value += Ner[NerColor].w[2] * Feature::boardColTransitions(currBotColor);
	value += Ner[NerColor].w[3] * Feature::boardBuriedHoles(currBotColor);
	value += Ner[NerColor].w[4] * Feature::boardWells(currBotColor);
	
	Util::recover();
	return value;
}

int blockType, typePosX, typePosY, typePosO, blockForEnemy, nextTypeForColor[2], currTypeForColor[2];

bool vis[MAPWIDTH+2][MAPHEIGHT+4][4];
struct node{int x,y,o;};

inline Tetris Determine(int currBotColor, int type, int NerColor)
{
	Tetris block(type, currBotColor), BestAction(type, currBotColor); BestAction.set(1,1,-2);
	
	queue<node>q; clr(vis,0);
	
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT, MAPHEIGHT+2) rep(o, 0, 3) if (block.set(x,y,o).onTop())
		vis[x][y][o]=1, q.push((node){x,y,o});
	
	double mx=-1e30, tmp;
	while (!q.empty())
	{
		int x=q.front().x, y=q.front().y, o=q.front().o; q.pop();
		block.set(x,y,o);
		if (!vis[x-1][y][o] && block.moveleft()) vis[x-1][y][o]=1, q.push((node){x-1,y,o});
		if (!vis[x+1][y][o] && block.moveright()) vis[x+1][y][o]=1, q.push((node){x+1,y,o});
		if (!vis[x][y-1][o] && block.movedown()) vis[x][y-1][o]=1, q.push((node){x,y-1,o});
		rep(i, 0, 3) if (!vis[x][y][i] && block.rotation(i)) vis[x][y][i]=1, q.push((node){x,y,i});
		
		if (!block.onGround()) continue; else if ((tmp=Value(block,currBotColor,NerColor))>mx)
			mx=tmp, BestAction.set(block.blockX,block.blockY,block.orientation);
		else if (tmp==mx && block.blockY<BestAction.blockY)
			mx=tmp, BestAction.set(block.blockX,block.blockY,block.orientation);
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
		typeCountForColor[color][type]++;
		
		Tetris block=Determine(color, type, color^1); if (block.orientation<0)
		{
			blockForEnemy = type; return;
		}
		if ((tmp=Value(block, color, color^1))<mn) mn=tmp, blockForEnemy=type;
		
		typeCountForColor[color][type]--;
	}
}

inline void PlayerAction(int currBotColor)
{
	Util::backup();
	
	int enemyColor=1^currBotColor;
	
	Tetris currB=Determine(currBotColor, currTypeForColor[currBotColor], currBotColor);
	Tetris enemyB=Determine(enemyColor, currTypeForColor[enemyColor], enemyColor);
	
	currB.place(); enemyB.place();
	Util::eliminate(0);
	Util::eliminate(1);
	int tmp=Util::transfer();
	
	if (tmp==-1) WorstDetermine(enemyColor); else
	{
		int maxCount = 0, minCount = 99;
		rep(type, 0, 6)
		{
			if (typeCountForColor[enemyColor][type] > maxCount)
				maxCount = typeCountForColor[enemyColor][type];
			if (typeCountForColor[enemyColor][type] < minCount)
				minCount = typeCountForColor[enemyColor][type];
		}
		
		for(int type=rand()%7; true; type=rand()%7) if (typeCountForColor[enemyColor][type] != maxCount || maxCount - minCount != 2) 
		{
			blockForEnemy=type;
			break;
		}
	}
	
	Util::recover();
	
	typePosX=currB.blockX, typePosY=currB.blockY, typePosO=currB.orientation;
}




 
int main()
{
	GetNer(0); GetNer(1);

	istream::sync_with_stdio(false);
	
	freopen("seed.txt", "r", stdin); 
	int seed; cin >> seed;
	srand(seed); fclose(stdin);
	freopen("seed.txt", "w", stdout);
	printf("%d\n", rand()*(RAND_MAX+1)+rand()+(int)(time(NULL)%10000));
	fclose(stdout);

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
		
		// �ȶ��Լ��������Ҳ�����Լ�����Ϊ
		// �Լ���������Լ������һ��
		// Ȼ��ģ�����һ�����ÿ�
		PlayerAction(0);
 
		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[0], 0);
		myBlock.set(typePosX, typePosY, typePosO);
		if (typePosO<0) ED|=1;
 
		// �Ҹ��Է�ʲô�����ţ�
		nextTypeForColor[1] = blockForEnemy;
 
		// Ȼ����Լ������룬Ҳ���ǶԷ�����Ϊ
		// ���и��Լ��������ǶԷ������һ��
		PlayerAction(1);
 
		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[1], 1);
		enemyBlock.set(typePosX, typePosY, typePosO);
		if (typePosO<0) ED|=2;
 
		// �Է�����ʲô�����ţ�
		nextTypeForColor[0] = blockForEnemy;
		
		if (ED) goto Final;
		
		myBlock.place();
		enemyBlock.place();
		typeCountForColor[0][nextTypeForColor[0]]++;
		typeCountForColor[1][nextTypeForColor[1]]++;
		
		/* freopen("con", "w", stdout);
		Util::printField();
		fclose(stdout); */
 
		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);
 
		// ����ת��
		if ((ED=Util::transfer())) goto Final;
	}
	
	Final:freopen("result.txt", "w", stdout);
	if (ED==1) puts("-1");
	if (ED==2) puts("1");
	if (ED==3) puts("0");
	fclose(stdout);
 
	return 0;
}