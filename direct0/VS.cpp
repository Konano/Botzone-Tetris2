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

int ED;
 
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
};

bool vis[MAPWIDTH+2][MAPHEIGHT+3][4];
struct node{int x,y,o;};

namespace Util
{
	// ΧһȦ���Ǻ�
	inline void init()
	{
		clr(gridInfo,0); clr(trans,0); clr(transCount,0); clr(maxHeight,0); clr(elimTotal,0); clr(elimCombo,0); clr(typeCountForColor,0);
		ED = 0;
		
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
		elimTotal[color] += elimBonus[count - hasBonus];
	}
 
	// ת��˫����ȥ���У�����-1��ʾ���������򷵻�����
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
		//cout << "~~��ǽ��[]���飬##���¿�" << endl;
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
	
	inline bool gameover(int ty0, int ty1)
	{
		int Final=3;
		
		Tetris block0(ty0, 0);
		
		queue<node>q; clr(vis,0);
		
		rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT, MAPHEIGHT+2) rep(o, 0, 3) if (block0.set(x,y,o).onTop())
			vis[x][y][o]=1, q.push((node){x,y,o});
		
		while (!q.empty())
		{
			int x=q.front().x, y=q.front().y, o=q.front().o; q.pop();
			block0.set(x,y,o);
			if (!vis[x-1][y][o] && block0.moveleft()) vis[x-1][y][o]=1, q.push((node){x-1,y,o});
			if (!vis[x+1][y][o] && block0.moveright()) vis[x+1][y][o]=1, q.push((node){x+1,y,o});
			if (!vis[x][y-1][o] && block0.movedown()) vis[x][y-1][o]=1, q.push((node){x,y-1,o});
			rep(i, 0, 3) if (!vis[x][y][i] && block0.rotation(i)) vis[x][y][i]=1, q.push((node){x,y,i});
			
			if (block0.onGround()) {Final-=1; break;}
		}
		
		Tetris block1(ty1, 1);
		
		while (!q.empty()) q.pop(); clr(vis,0);
		
		rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT, MAPHEIGHT+2) rep(o, 0, 3) if (block1.set(x,y,o).onTop())
			vis[x][y][o]=1, q.push((node){x,y,o});
		
		while (!q.empty())
		{
			int x=q.front().x, y=q.front().y, o=q.front().o; q.pop();
			block1.set(x,y,o);
			if (!vis[x-1][y][o] && block1.moveleft()) vis[x-1][y][o]=1, q.push((node){x-1,y,o});
			if (!vis[x+1][y][o] && block1.moveright()) vis[x+1][y][o]=1, q.push((node){x+1,y,o});
			if (!vis[x][y-1][o] && block1.movedown()) vis[x][y-1][o]=1, q.push((node){x,y-1,o});
			rep(i, 0, 3) if (!vis[x][y][i] && block1.rotation(i)) vis[x][y][i]=1, q.push((node){x,y,i});
			
			if (block1.onGround()) {Final-=2; break;}
		} 
		return ED|=Final;
	}
}

inline int Game()
{
	Util::init();
	
	int turnID = 0, blockType = rand()%7;
	int nextTypeForColor[2];
	
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][blockType]++;
	typeCountForColor[1][blockType]++;
	
	freopen("input0.txt", "w", stdout); printf("%d %d\n", blockType, 0); fclose(stdout);
	freopen("input1.txt", "w", stdout); printf("%d %d\n", blockType, 1); fclose(stdout);
	
	while (++turnID)
	{
		int currTypeForColor[2] = { nextTypeForColor[0], nextTypeForColor[1] };
		
		system("player0.exe");
		system("player1.exe");
		
		freopen("player0_action.txt", "r", stdin);
		int b0, x0, y0, o0;
		scanf("%d%d%d%d", &b0, &x0, &y0, &o0);
		fclose(stdin);
 
		// �ҵ�ʱ����һ���䵽�� x y o��
		Tetris myBlock(currTypeForColor[0], 0);
		if (!myBlock.set(x0, y0, o0).place()) ED|=1;
 
		// �Ҹ��Է�ʲô�����ţ�
		typeCountForColor[1][b0]++;
		nextTypeForColor[1] = b0;

		freopen("player1_action.txt", "r", stdin);
		int b1, x1, y1, o1;
		scanf("%d%d%d%d", &b1, &x1, &y1, &o1);
		fclose(stdin);
 
		// �Է���ʱ����һ���䵽�� x y o��
		Tetris enemyBlock(currTypeForColor[1], 1);
		if (!enemyBlock.set(x1, y1, o1).place()) ED|=2;
 
		// �Է�����ʲô�����ţ�
		typeCountForColor[0][b1]++;
		nextTypeForColor[0] = b1;
		
		freopen("con", "w", stdout);
		Util::printField();
		fclose(stdout);
 
		// �����ȥ
		Util::eliminate(0);
		Util::eliminate(1);
 
		// ����ת��
		ED|=Util::transfer();
		
		freopen("input0.txt", "a", stdout); printf("%d %d %d %d\n%d %d %d %d\n", b0, x0, y0, o0, b1, x1, y1, o1); fclose(stdout);
		freopen("input1.txt", "a", stdout); printf("%d %d %d %d\n%d %d %d %d\n", b1, x1, y1, o1, b0, x0, y0, o0); fclose(stdout);
		
		if (Util::gameover(nextTypeForColor[0], nextTypeForColor[1])) return (ED&1?-1:0)+(ED&2?1:0);
	}
	return 0;
}

int r[1000009];

int main()
{
	istream::sync_with_stdio(false);
	srand(time(NULL));
	
	int n=0; double sum=0; while (true)
	{
		sum+=1.0*((r[++n]=Game())+1)/2;
		
		freopen("con", "w", stdout);
		printf("MatchNum: %d\n", n);
		puts(r[n]>0?"Left Win!":(r[n]<0?"Right Win!":"Half!"));
		printf("%.6lf%%\n", sum/n*100);
		fclose(stdout);
		
		freopen("con", "r", stdin);
		system("pause");
	}
}