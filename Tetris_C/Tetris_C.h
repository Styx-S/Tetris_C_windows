#pragma once
#include "stdafx.h"
#define MM_X(x) (x + pBlock->xBlockLeft)
#define MM_Y(y) (y + pBlock->yBlockTop)
#define TETRIS_COLOR_0 RGB(65,105,255) // 蓝色 	RoyalBlue
#define TETRIS_COLOR_1 RGB(0,255,255) // 青色 	Cyan
#define TETRIS_COLOR_2 RGB(0,255,127) // 绿色 MediumSpringGreen
#define TETRIS_COLOR_3 RGB(255,255,0) // 黄色 Yellow
#define TETRIS_COLOR_4 RGB(255,0,0) // 红色 Red
#define TETRIS_COLOR_5 RGB(69,169,169) // 灰色 DarkGray
typedef struct {
	int xBlockLeft, yBlockTop;
	int size;
	bool *pbBlockArea;
	COLORREF color;
} GameBlock;

typedef struct {
	int width, height;
	int score;
	COLORREF *pcrGameArea;
	bool bGameOver;
} GameBoard;

bool BlockDown(GameBoard *pBoard, GameBlock **pBlock);
bool BlockRotate(GameBoard *pBoard, GameBlock *pBlock);
bool BlockLeft(GameBoard *pBoard, GameBlock *pBlock);
bool BlockRight(GameBoard *pBoard, GameBlock *pBlock);
void makeBoard(GameBoard **pBoard, int width, int height);
bool makeBlock(GameBoard *pBoard, GameBlock **pBlock);

#include "resource.h"
