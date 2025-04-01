/**
 * @file player.h
 * @author yangboyang@jisuanke.com
 * @copyright jisuanke.com
 * @date 2021-07-01
 */ 
#define MAXSIZE 13
#define INFINITE 0x7fffffff
#define MAXWIDTH 4
#define MAXDEPTH 6
#include <string.h>
#include "../include/playerbase.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
int xvertex[4], yvertex[4];
int xCposition[2], yCposition[2]; 
char score_graph[MAXSIZE][MAXSIZE];
int step[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
char mat[MAXSIZE][MAXSIZE];
int priority[MAXSIZE][MAXSIZE], max_x, max_y;
int movPriority, scorePriority;
int score;
int inChessboard(int x, int y, struct Player *player);
int getstable(struct Player *player,char now_mat[MAXSIZE][MAXSIZE]);
int is_valid(struct Player *player, int posx, int posy,char now_mat[][MAXSIZE]);
void init(struct Player *player);
struct Point place(struct Player *player);
int chessAmount(struct Player *player,char now_mat[MAXSIZE][MAXSIZE]);
int minmax(struct Player *player, int step,char now_mat[MAXSIZE][MAXSIZE],char op_mat[MAXSIZE][MAXSIZE],int border, int is_max);
void changeGraph(struct Player *player,int posx,int posy,char my_graph[MAXSIZE][MAXSIZE],char opponent_graph[MAXSIZE][MAXSIZE]);
void copyGraph(char graph1[][MAXSIZE],char graph2[][MAXSIZE], int row_amount);
int getPriority(struct Player *player,char now_mat[MAXSIZE][MAXSIZE]);
int movSpace(struct Player *player,char now_mat[MAXSIZE][MAXSIZE],char op_mat[MAXSIZE][MAXSIZE]);

int inChessboard(int x, int y, struct Player *player) {
	return x>=0&&x<player->row_cnt&&y>=0&&y<player->col_cnt;
}

int chessAmount(struct Player *player, char graph[MAXSIZE][MAXSIZE]) {
    int chess_amount = 0;
    for (int i = 0; i < player->row_cnt; i++) {
        for (int j = 0; j < player->col_cnt; j++) {
            chess_amount += (graph[i][j]=='O' || graph[i][j]=='o') ? 1 : 0;
        }
    }
    return chess_amount; 
}

int getstable(struct Player *player,char now_mat[MAXSIZE][MAXSIZE]) {
    int stable[3] = {0, 0, 0};
    // 用来存储stable值，stable[0]用来存储占据顶点的数量, stable[1]用来存储边上占据的稳固点的数量, stable[2]用来存储内部的稳固点的数量

    int cind1[4] = {0};
    int cind2[4] = {0};
    cind1[2]=cind1[3]=player->col_cnt-1;
    cind2[1]=cind2[2]=player->col_cnt-1; // 检查顶点是否占据的辅助函数

    int inc1[4] = {0, 1, 0, -1};
    int inc2[4] = {1, 0, -1, 0};

    int stop[4] = {0}; // stop四个变量分别存储上下左右四条边中已经稳固的点到了第几个

    int i, j;
    for (i = 0; i < 4; i++) {
        if (now_mat[cind1[i]][cind2[i]]=='O') {
            stop[i] = 1;
            stable[0] += 1;
            for (j = 1; j < player->col_cnt; j++) {
                if (now_mat[cind1[i]+inc1[i]*j][cind2[i]+inc2[i]*j]!='O') break;
                else {
                    stop[i] = j + 1;
                    stable[1] += 1;
                }
            }
        }
    } // 检查边缘

    for (i = 0; i < 4; i++) {
        if (now_mat[cind1[i]][cind2[i]]=='O') {
            for (j = 1; j <player->col_cnt - stop[(i+3)%4]; j++) {
                if (now_mat[cind1[i]-inc1[(i+3)%4]*j][cind2[i]-inc2[(i+3)%4]*j]!='O') break;
                else stable[1] += 1;
            }
        }
    }

    int colfull[13]={0}; // 已满的列的数量
    int rowfull[13]={0}; // 已满的行的数量
    int diag1full[26]={0}; // 已满的左上右下对角线数量
    int diag2full[26]={0}; // 已满的右上左下对角线数量
    for(i=0;i<player->col_cnt;i++) {
        for(j=0;j<player->row_cnt;j++) {
            if(now_mat[i][j]!='O'&&now_mat[i][j]!='o') break;
        }
        if(j==player->col_cnt) rowfull[i]=1;
    } //搜寻已满的行的数量
    for (i = 0; i < player->row_cnt; i++) {
        for(j=0;j<player->col_cnt;j++) {
            if(now_mat[j][i]!='O'&&now_mat[j][i]!='o') break;
        }
        if(j==player->col_cnt) colfull[i]=1;
    } //搜寻已满的列的数量

    for(i=0;i<player->col_cnt*2-1;i++) {
        int diacnt=player->col_cnt-abs((player->col_cnt*2-2)/2-i); // 对角线上点的数量
        int startx,starty;
        // 起始点在左边和上边
        if(i<player->col_cnt-1) {
            startx=player->col_cnt-1-i;
            starty=0;
        }
        else {
            startx=0;
            starty=i-player->col_cnt+1;
        }
        //设定起始位置
        for(j=0;j<diacnt;j++) {
            if(now_mat[startx+j][starty+j]!='O'&&now_mat[startx+j][starty+j]!='o')
                break;
        } // 从左上向右下遍历该对角线的每一个点
        if(j==diacnt) diag1full[i]=1;
            //判断是否为满对角线

        // 起始点在右边和上边
        if(i<player->col_cnt-1){
            startx=player->col_cnt-1-i;
            starty=player->col_cnt-1;
        }
        else {
            startx=0;
            starty=2*player->col_cnt-2-i;
        }
        //设定起始位置
        for(j=0;j<diacnt;j++) {
            if(now_mat[startx+j][starty-j]!='O'&&now_mat[startx+j][starty-j]!='o')
                break;
        } // 从右上向左下遍历该对角线的每一个点
        if(j==diacnt) diag2full[i]=1;
            //判断是否为满对角线
    }
    for(i=1;i<player->col_cnt-1;i++) {
        for(j=1;j<player->col_cnt-1;j++) {
            int diag1=j-i+player->col_cnt-1;
            int diag2=2*player->col_cnt-2-j-i;
            if(now_mat[i][j]=='O'&&colfull[j]&&rowfull[i]&&diag1full[diag1]&&diag2full[diag2]) stable[2]++;
        }
    }// 遍历每一个点，如果该点的八个方向都满了，证明该点稳定，stable[2]++
    return stable[0]+stable[1]+stable[2];
}

void changeGraph(struct Player *player,int posx,int posy,char my_graph[MAXSIZE][MAXSIZE],char opponent_graph[MAXSIZE][MAXSIZE])
{
    for (int dir = 0;  dir < 8; dir++) {
        int x = posx + step[dir][0];
        int y = posy + step[dir][1];
        if (!inChessboard(x, y, player) || my_graph[x][y]!='o') continue;
        while (true) {
            x += step[dir][0];
            y += step[dir][1];
            if (!inChessboard(x, y, player) || (my_graph[x][y] >= '1' && my_graph[x][y] <= '9')) break;
            if (my_graph[x][y] == 'O')  {
                while (!(x==posx&&y==posy)) {
                    x -= step[dir][0];
                    y -= step[dir][1];
                    my_graph[x][y] = 'O';
                    opponent_graph[x][y] = 'o';
                }
                break;
            }
        }
    }
}

void copyGraph(char a[][MAXSIZE],char b[][MAXSIZE], int row_amount) {
    for(int i = 0; i < row_amount; i++) strncpy(a[i], b[i], MAXSIZE);
}

int movSpace(struct Player *player,char now_mat[MAXSIZE][MAXSIZE],char op_mat[MAXSIZE][MAXSIZE])
{
    int my_space = 0, opponent_space = 0;
    for (int i=0; i<player->row_cnt; i++) {
         for (int j=0; j<player->col_cnt; j++) {
            if (is_valid(player,i,j,now_mat)) my_space++;
            if (is_valid(player,i,j,op_mat)) opponent_space++;
         }
    }
    return my_space-opponent_space;
}

int getPriority(struct Player *player,char now_mat[MAXSIZE][MAXSIZE])
{
    int prioritys = 0;
    int my_chess = 0, opponent_chess = 0;
    score=0;
    for(int i=0;i<player->row_cnt;i++) {
        for(int j=0;j<player->col_cnt;j++) {
            if(now_mat[i][j]=='O') {
                prioritys += priority[i][j];
                my_chess++;
                score+=score_graph[i][j];
            } // 累加棋盘上每一个我方占有的位置的优先级，计算出我方的棋子数目
            else if(now_mat[i][j]=='o') {
                opponent_chess++;
            } // 计算敌方的棋子数目
        }
    }
    if(my_chess==0) return -INFINITE;
    else if(opponent_chess==0) return INFINITE;
    else return prioritys;
}  // 获取优先级总和

int minmax(struct Player *player, int step,char now_mat[MAXSIZE][MAXSIZE],char op_mat[MAXSIZE][MAXSIZE],int border, int is_max) {
    // 基情况
    if(step>MAXDEPTH) {
        return getPriority(player,now_mat)+movPriority*movSpace(player,now_mat,op_mat)+50*getstable(player,now_mat)+scorePriority*score;
    }

    // 递归情况
    int x[MAXWIDTH+1],y[MAXWIDTH+1];
    for(int i=0;i<MAXDEPTH;i++) {
        x[i]=y[i]=-1;
    } // 全部赋值为-1
    for(int i=0;i<player->row_cnt;i++) {
         for(int j=0;j<player->row_cnt;j++) {
            if(is_valid(player,i,j,now_mat)) {
                int k=MAXWIDTH-1;
                while(k>=0&&(x[k]==-1||priority[i][j]>priority[x[k]][y[k]])) {
                    x[k+1]=x[k];
                    y[k+1]=y[k];
                    k--;
                }
                x[k+1]=i;
                y[k+1]=j;
            }
        }
    } // 遍历假想地图上的每一个点，设置地图上优先级最高的五个点，存入x[]y[]中。

    if(x[0]==-1) { // 无点可放子
        if(step==1) { // 如果刚开始就无子可放，洗洗睡吧
            max_x=-1,max_y=-1;
            return 0;
        }
        // 如果不是第一步，就相当于跳过该步，继续走着
        char next_graph[MAXSIZE][MAXSIZE]; // 下一步的我的地图
        char next_ograph[MAXSIZE][MAXSIZE]; //下一步的对手的地图
        copyGraph(next_graph,now_mat, player->row_cnt); 
        copyGraph(next_ograph, op_mat, player->row_cnt); // copy 下一步的地图
        return is_max ? minmax(player,step+1,next_ograph,next_graph,-INFINITE, is_max^1) : minmax(player,step+1,next_ograph,next_graph,INFINITE,is_max^1);
    }
    int chose_value = is_max ? -INFINITE : INFINITE;
    for(int i=0;i<MAXDEPTH&&x[i]!=-1;i++) {
        char next_graph[MAXSIZE][MAXSIZE];
        char next_ograph[MAXSIZE][MAXSIZE];
        copyGraph(next_graph, now_mat, player->row_cnt);
        copyGraph(next_ograph, op_mat, player->row_cnt); // 创建并copy下一步的地图
        changeGraph(player,x[i],y[i],next_graph,next_ograph); // 更改地图， 下子
        int value = minmax(player,step+1,next_ograph,next_graph,chose_value, is_max^1);

        if(is_max) { 
             if (value>border) return INFINITE;  // alpha >= beta 剪枝
             if (value>chose_value) {
                chose_value=value;
                if(step==1) {
                    max_x=x[i];
                    max_y=y[i];
                }
            } // 返回较大的那个
        }
        else {
            if(value<border) return -INFINITE; // 剪枝
            if(value<chose_value) {
                chose_value=value;
            } // 返回较小的那个
        }
    } // 遍历每一个可放子的点
    return chose_value;
}

int is_valid(struct Player *player, int posx, int posy, char now_mat[][MAXSIZE]) {
    if (!inChessboard(posx, posy, player) || now_mat[posx][posy] == 'o' || now_mat[posx][posy] == 'O') return false;
    for (int dir = 0;  dir < 8; dir++) {
        int x = posx + step[dir][0];
        int y = posy + step[dir][1];
        if (!inChessboard(x, y, player) || now_mat[x][y]!='o') continue;
        while (true) {
            x += step[dir][0];
            y += step[dir][1];
            if (!inChessboard(x, y, player) || (now_mat[x][y] >= '1' && now_mat[x][y] <= '9')) break;
            if (now_mat[x][y] == 'O') return true;
        }
    }
    return false;
}

void init(struct Player *player) {
	for (int i = 0; i < player->row_cnt; i++) {
		strncpy(score_graph[i], player->mat[i], player->col_cnt);
		score_graph[i][player->col_cnt] = '\0';
	}
	for(int i=0;i<player->row_cnt;i++)
        for(int j=0;j<player->col_cnt;j++)
            mat[i][j]=player->mat[i][j];
    xvertex[0] = yvertex[0] = 0; 
    xvertex[2] = yvertex[2] = 1;
    xvertex[1] = player->row_cnt-1, yvertex[1] = player->col_cnt-1;
    xvertex[3] = player->row_cnt-2, yvertex[3] = player->col_cnt-2;
    xCposition[0] = yCposition[0] = 2;
    xCposition[1] = player->row_cnt-3, yCposition[1] = player->col_cnt-3;
    for (int i = 0; i < player->row_cnt; i++) {
        for (int j = 0; j < player->col_cnt; j++) {
            priority[i][j] = 4;
        }
    } // 设置所有点的权值
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < player->row_cnt; j++) {
            priority[xvertex[i]][j] = priority[j][yvertex[i]] = 25;
        }
        for (int j = 1; j < player->row_cnt-1; j++) {
            priority[xvertex[i+2]][j] = priority[j][yvertex[i+2]] = 1;
        }
    } // 设置最外圈和次外圈的权值
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            priority[xvertex[i]][yvertex[j+2]] = -50;
            priority[xvertex[j+2]][yvertex[i]] = -50;
        }
    } // 设置顶点周围的权值
    for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
				priority[xvertex[i]][yCposition[j]] = 50;
                priority[xCposition[i]][yvertex[j]] = 50;
		}
	} // 设置Cposition
    priority[2][2] = priority[2][yCposition[1]] = priority[xCposition[1]][2] = priority[xCposition[1]][yCposition[1]] = 15; // 设置XXposition
    priority[1][1] = priority[1][yvertex[3]] = priority[xvertex[3]][1] = priority[xvertex[3]][yvertex[3]] = -80;//设置Xposition
    priority[0][0] = priority[0][yvertex[1]] = priority[xvertex[1]][0] = priority[xvertex[1]][yvertex[1]] = 10000;//设置顶点
}

struct Point place(struct Player *player) {
    char op_mat[MAXSIZE][MAXSIZE],now_mat[MAXSIZE][MAXSIZE];
    if (player->row_cnt == 8) movPriority = 15;
    else if (player->row_cnt == 10) movPriority = 12;
    else movPriority=10; // 设立行动权值

    for(int i=0;i<player->row_cnt;i++) {
        for(int j=0;j<player->col_cnt;j++) {
            now_mat[i][j]=player->mat[i][j];
            if(player->mat[i][j]=='O') op_mat[i][j]='o';
            else if(player->mat[i][j]=='o') op_mat[i][j]='O';
            else op_mat[i][j]=player->mat[i][j];
        }
    } // 创立两张假想地图

    int chess = chessAmount(player,now_mat);
    if(player->row_cnt*player->row_cnt-chess<=player->row_cnt-4) scorePriority = 4;
    else scorePriority=0;

    minmax(player,1,now_mat,op_mat,INFINITE,1);
    return initPoint(max_x,max_y);
}