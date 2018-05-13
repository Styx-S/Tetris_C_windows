#include "stdafx.h"
#include "Tetris_C.h"
static bool Blockvisitor(GameBoard *pBoard, GameBlock *pBlock, bool(*pf)(int, int, GameBoard*, GameBlock*)) {
	bool BlockTemp[25];
	int i, j;
	// 防止旋转等操作导致重复计数
	for (i = 0; i < pBlock->size; i++) {
		for (j = 0; j < pBlock->size; j++) {
			BlockTemp[i + j * pBlock->size] = pBlock->pbBlockArea[i + j * pBlock->size];
		}
	}
	for (i = 0; i < pBlock->size; i++) {
		for (j = 0; j < pBlock->size; j++) {
			if (!BlockTemp[i + j * pBlock->size])
				continue;
			if (!pf(i, j, pBoard, pBlock))
				return false;
		}
	}
	return true;
}
// 注意方块中点坐标到棋盘上坐标的映射 MM_X MM_Y
static bool clearBlock(int x, int y, GameBoard *pBoard, GameBlock *pBlock) {
	pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] = 0;
	return true;
}
// Game over!!!
static bool canDrawBlock(int x, int y, GameBoard *pBoard, GameBlock *pBlock) {
	if (pBoard->pcrGameArea[MM_X(x) + MM_Y(y) * pBoard->width])
		return false;
	return true;
}
static bool drawBlock(int x, int y, GameBoard *pBoard, GameBlock *pBlock) {
	pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] = pBlock->color;
	return true;
}

static bool canBlockDown(int x, int y, GameBoard *pBoard, GameBlock *pBlock) {
	// 有效点不能超过棋盘边界
	if (MM_Y(y) + 1 >= pBoard->height)
		return false;
	// 如果下方有不是自身的方块，那么不能向下移动
	// 调用时判断结果，产生新的图形
	if (pBoard->pcrGameArea[MM_X(x) + (MM_Y(y) + 1) * pBoard->width] &&
		// 注意判断是否越界
		((x + (y + 1)*pBlock->size) >= pow(pBlock->size, 2) |
			pBlock->pbBlockArea[x + (y + 1) * pBlock->size] == 0))
		return false;
	else return true;
}
// 使所有方块下移，只用于消去一行后
static void BoardDown(GameBoard *pBoard,int row) {
	for (int i = 0; i<pBoard->width; i++) {
		for (int j = row; j >= 1; j--) {
			pBoard->pcrGameArea[i + j * pBoard->width] =
				pBoard->pcrGameArea[i + (j - 1)*pBoard->width];
		}
	}
}
// 消去整行 (注意：并未改变方块内部存储)
static void eliminateBlock(GameBoard *pBoard, GameBlock *pBlock, int row) {
	if (row == pBlock->size)
		return;
	int currentRow = pBlock->yBlockTop + (pBlock->size - 1) - row;
	for (int i = 0; i < pBoard->width; i++) {
		if (pBoard->pcrGameArea[i + currentRow * pBoard->width] == 0) {
			eliminateBlock(pBoard, pBlock, row + 1);
			return;
		}
	}
	for (int i = 0; i < pBoard->width; i++) {
		pBoard->pcrGameArea[i + currentRow * pBoard->width] = 0;
	}
	eliminateBlock(pBoard, pBlock, row+1);
	BoardDown(pBoard, currentRow);
	return;
}
bool makeBlock(GameBoard *pBoard, GameBlock **pBlock);
bool BlockDown(GameBoard *pBoard, GameBlock **pBlock) {
	// 判断是否到底
	if (!Blockvisitor(pBoard, *pBlock, canBlockDown)) {
		eliminateBlock(pBoard, *pBlock, 0);

		if (makeBlock(pBoard, pBlock))
			return false;
		else return true;
	}
	Blockvisitor(pBoard, *pBlock, clearBlock);
	(*pBlock)->yBlockTop++;
	Blockvisitor(pBoard, *pBlock, drawBlock);
	return true;
}
static bool canPointRotate33(int i, int j, GameBoard *pBoard, GameBlock *pBlock) {
	int x = j, y = 2 - i;
	if (x < 0 | x >= pBlock->size | y < 0 | y >= pBlock->size)
		return false;
	if (pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] && pBlock->pbBlockArea[x + y * pBlock->size] == 0)
		return false;
	else return true;
}
// 注意不要将已旋转的点再次旋转；旋转来的点，也不能取消
static bool PointRotate33(int i, int j, GameBoard *pBoard, GameBlock *pBlock) {
	int x = j, y = 2 - i;
	if (!pBoard->pcrGameArea[MM_X(i) + MM_Y(j)*pBoard->width])
		pBlock->pbBlockArea[i + j * pBlock->size] = FALSE;
	pBlock->pbBlockArea[x + y * pBlock->size] = TRUE;
	// 因此rotate方法不需要调用draw
	pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] = pBlock->color;
	return true;
}
bool BlockRotate(GameBoard *pBoard, GameBlock *pBlock) {
	// 2*2 只有正方形，不旋转
	if (pBlock->size == 2)
		return false;
	// 3*3 变换矩阵为[0,-1,0;1,0,0;0,2,1]
	else if (pBlock->size == 3) {
		if (Blockvisitor(pBoard, pBlock, canPointRotate33)) {
			Blockvisitor(pBoard, pBlock, clearBlock);
			Blockvisitor(pBoard, pBlock, PointRotate33);
			return true;
		}
		else return false;
	}
	// 竖条单独旋转
	else if (pBlock->size == 4) {
		return true;
	}
	else return false;
}

static bool canBlockLeft(int x, int y, GameBoard *pBoard, GameBlock *pBlock) {
	if (MM_X(x) <= 0)
		return false;
	// 如果左侧有不是自身的方块，那么不能向左移动
	if (pBoard->pcrGameArea[MM_X(x) - 1 + MM_Y(y) * pBoard->width] &&
		((x - 1 + y * pBlock->size) < 0 |
			pBlock->pbBlockArea[x - 1 + y * pBlock->size] == 0))
		return false;
	else return true;
}
bool BlockLeft(GameBoard *pBoard, GameBlock *pBlock) {
	if (!Blockvisitor(pBoard, pBlock, canBlockLeft))
		return false;
	Blockvisitor(pBoard, pBlock, clearBlock);
	pBlock->xBlockLeft--;
	Blockvisitor(pBoard, pBlock, drawBlock);
	return true;
}

static bool canBlockRight(int x, int y, GameBoard *pBoard, GameBlock *pBlock) {
	if (MM_X(x) + 1 >= pBoard->width)
		return false;
	// 如果右侧有不是自身的方块，那么不能向右移动
	if (pBoard->pcrGameArea[MM_X(x) + 1 + MM_Y(y) * pBoard->width] &&
		(x + 1 + y * pBlock->size >= pow(pBlock->size, 2)
			| pBlock->pbBlockArea[x + 1 + y * pBlock->size] == 0))
		return false;
	else return true;
}
bool BlockRight(GameBoard *pBoard, GameBlock *pBlock) {
	if (!Blockvisitor(pBoard, pBlock, canBlockRight))
		return false;
	Blockvisitor(pBoard, pBlock, clearBlock);
	pBlock->xBlockLeft++;
	Blockvisitor(pBoard, pBlock, drawBlock);
	return true;
}

void makeBoard(GameBoard **pBoard, int width, int height) {
	if (*pBoard != NULL)
	{
		free((*pBoard)->pcrGameArea);
		free((*pBoard));
	}
	(*pBoard) = (GameBoard*)malloc(sizeof(GameBoard));
	(*pBoard)->height = height;
	(*pBoard)->width = width;
	(*pBoard)->pcrGameArea = (COLORREF*)malloc(sizeof(COLORREF)*height*width);
	memset((*pBoard)->pcrGameArea, 0, sizeof(COLORREF)*height*width);
	(*pBoard)->bGameOver = false;
	(*pBoard)->score = 0;
}
static void initBlock(GameBlock **pBlock, int size) {
	(*pBlock)->size = size;
	(*pBlock)->pbBlockArea = (bool*)malloc(sizeof(bool)*pow((*pBlock)->size, 2));
	memset((*pBlock)->pbBlockArea, 0, sizeof(bool)*pow((*pBlock)->size, 2));
}
static COLORREF initColor() {
	switch (rand() % 6) {
	case 1:
		return TETRIS_COLOR_1;
	case 2:
		return TETRIS_COLOR_2;
	case 3:
		return TETRIS_COLOR_3;
	case 4:
		return TETRIS_COLOR_4;
	case 5:
		return TETRIS_COLOR_5;
	default:
		return TETRIS_COLOR_0;
	}
}
bool makeBlock(GameBoard *pBoard, GameBlock **pBlock) {
#define BLOCK (*pBlock)->pbBlockArea
	if (*pBlock != NULL)
	{
		free((*pBlock)->pbBlockArea);
		free(*pBlock);
	}
	*pBlock = (GameBlock*)malloc(sizeof(GameBlock));
	switch (rand() % 7) {
		// 左J
	case 1:
		initBlock(pBlock, 3);
		BLOCK[0] = BLOCK[0 + 1 * 3] = BLOCK[1 + 1 * 3] = BLOCK[2 + 1 * 3] = 1;
		break;
		// 右J
	case 2:
		initBlock(pBlock, 3);
		BLOCK[2] = BLOCK[0 + 1 * 3] = BLOCK[1 + 1 * 3] = BLOCK[2 + 1 * 3] = 1;
		break;
		// T
	case 3:
		initBlock(pBlock, 3);
		BLOCK[1] = BLOCK[0 + 1 * 3] = BLOCK[1 + 1 * 3] = BLOCK[2 + 1 * 3] = 1;
		break;
		// 左Z
	case 4:
		initBlock(pBlock, 3);
		BLOCK[1] = BLOCK[2] = BLOCK[0 + 1 * 3] = BLOCK[1 + 1 * 3] = 1;
		break;
		// 右Z
	case 5:
		initBlock(pBlock, 3);
		BLOCK[0] = BLOCK[1] = BLOCK[1 + 1 * 3] = BLOCK[2 + 1 * 3] = 1;
		break;
		// I
	case 6:
		initBlock(pBlock, 4);
		BLOCK[1] = BLOCK[1 + 1 * 4] = BLOCK[1 + 2 * 4] = BLOCK[1 + 3 * 4] = 1;
		break;
	default:
		initBlock(pBlock, 2);
		(*pBlock)->pbBlockArea[0] = (*pBlock)->pbBlockArea[1] = (*pBlock)->pbBlockArea[2] = (*pBlock)->pbBlockArea[3] = 1;
		break;
	}
	(*pBlock)->yBlockTop = 0;
	(*pBlock)->xBlockLeft = pBoard->width / 2 - 1;
	(*pBlock)->color = initColor();
	if (!Blockvisitor(pBoard, *pBlock, canDrawBlock)) {
		pBoard->bGameOver = true;
		return false;
	}
	Blockvisitor(pBoard, *pBlock, drawBlock);
	return true;
}