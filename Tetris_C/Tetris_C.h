#pragma once
#include "stdafx.h"
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
	GameBlock *pCurrentBlock;
	GameBlock *pNextBlock;
	bool bGameOver;
} GameBoard;

bool BlockDown(GameBoard *pBoard);
bool BlockRotate(GameBoard *pBoard);
bool BlockLeft(GameBoard *pBoard);
bool BlockRight(GameBoard *pBoard);
void makeBoard(GameBoard **pBoard, int width, int height);

#include "resource.h"
