#pragma once
#include "stdafx.h"
#define MM_X(x) (x + pBlock->xBlockLeft)
#define MM_Y(y) (y + pBlock->yBlockTop)
typedef struct {
	int xBlockLeft, yBlockTop;
	int size;
	bool *pbBlockArea;
	COLORREF color;
} GameBlock;

typedef struct {
	int width, height;
	COLORREF *pcrGameArea;
	bool bGameOver;
} GameBoard;

bool makeBlock(GameBoard *pBoard, GameBlock **pBlock);
bool BlockDown(GameBoard *pBoard, GameBlock **pBlock);
bool BlockRotate(GameBoard *pBoard, GameBlock *pBlock);
bool BlockLeft(GameBoard *pBoard, GameBlock *pBlock);
bool BlockRight(GameBoard *pBoard, GameBlock *pBlock);
void makeBoard(GameBoard **pBoard, int width, int height);
bool makeBlock(GameBoard *pBoard, GameBlock **pBlock);

#include "resource.h"
