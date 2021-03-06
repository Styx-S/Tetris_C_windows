#include "stdafx.h"
#include "Tetris_C.h"

#define BLOCK pBoard->pCurrentBlock
#define MM_X(x) (x + BLOCK->xBlockLeft)
#define MM_Y(y) (y + BLOCK->yBlockTop)

static bool Blockvisitor(GameBoard *pBoard, bool(*pf)(int, int, GameBoard*)) {
	bool BlockTemp[25];
	int i, j;
	// 防止旋转等操作导致重复计数
	for (i = 0; i < BLOCK->size; i++) {
		for (j = 0; j < BLOCK->size; j++) {
			BlockTemp[i + j * BLOCK->size] = BLOCK->pbBlockArea[i + j * BLOCK->size];
		}
	}
	for (i = 0; i < BLOCK->size; i++) {
		for (j = 0; j < BLOCK->size; j++) {
			if (!BlockTemp[i + j * BLOCK->size])
				continue;
			if (!pf(i, j, pBoard))
				return false;
		}
	}
	return true;
}
// 注意方块中点坐标到棋盘上坐标的映射 MM_X MM_Y
static bool clearBlock(int x, int y, GameBoard *pBoard) {
	pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] = 0;
	return true;
}
// Game over!!!
static bool canDrawBlock(int x, int y, GameBoard *pBoard) {
	if (pBoard->pcrGameArea[MM_X(x) + MM_Y(y) * pBoard->width])
		return false;
	return true;
}
static bool drawBlock(int x, int y, GameBoard *pBoard) {
	pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] = BLOCK->color;
	return true;
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
static void eliminateBlock(GameBoard *pBoard,  int row, double score) {
	if (row == BLOCK->size)
		return;
	int currentRow = BLOCK->yBlockTop + (BLOCK->size - 1) - row;
	if (currentRow >= pBoard->height) {
		eliminateBlock(pBoard, row + 1, score);
		return;
	}
	for (int i = 0; i < pBoard->width; i++) {
		if (pBoard->pcrGameArea[i + currentRow * pBoard->width] == 0) {
			eliminateBlock(pBoard, row + 1, score);
			return;
		}
	}
	for (int i = 0; i < pBoard->width; i++) {
		pBoard->pcrGameArea[i + currentRow * pBoard->width] = 0;
	}
	// 在此处修改消除多层分数：
	int currentScore = score * 2; 
	pBoard->score += currentScore;
	eliminateBlock(pBoard, row+1, currentScore);
	BoardDown(pBoard, currentRow);
	return;
}

static bool canBlockDown(int x, int y, GameBoard *pBoard) {
	// 有效点不能超过棋盘边界
	if (MM_Y(y) + 1 >= pBoard->height)
		return false;
	// 如果下方有不是自身的方块，那么不能向下移动
	// 调用时判断结果，产生新的图形
	if (pBoard->pcrGameArea[MM_X(x) + (MM_Y(y) + 1) * pBoard->width] &&
		// 注意判断是否越界
		((x + (y + 1)*BLOCK->size) >= pow(BLOCK->size, 2) |
			BLOCK->pbBlockArea[x + (y + 1) * BLOCK->size] == 0))
		return false;
	else return true;
}
bool changeBlock(GameBoard *pBoard);
bool BlockDown(GameBoard *pBoard) {
	// 判断是否到底
	if (!Blockvisitor(pBoard, canBlockDown)) {
		eliminateBlock(pBoard, 0, 0.5);

		if (changeBlock(pBoard))
			return false;
		else return true;
	}
	Blockvisitor(pBoard, clearBlock);
	BLOCK->yBlockTop++;
	Blockvisitor(pBoard, drawBlock);
	return true;
}
static bool canPointRotate33(int i, int j, GameBoard *pBoard) {
	int x = j, y = 2 - i;
	if (x < 0 | x >= BLOCK->size | y < 0 | y >= BLOCK->size)
		return false;
	// 在左移右移时可能导致方块区域超出棋盘区域时仍然可用，此时要防止旋转时越过棋盘边界
	if (MM_X(x) < 0 | MM_X(x) >= pBoard->width | MM_Y(y) < 0 | MM_Y(y) >= pBoard->height)
		return false;
	if (pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] && BLOCK->pbBlockArea[x + y * BLOCK->size] == 0)
		return false;
	else return true;
}
// 注意不要将已旋转的点再次旋转；旋转来的点，也不能取消
static bool PointRotate33(int i, int j, GameBoard *pBoard) {
	int x = j, y = 2 - i;
	if (!pBoard->pcrGameArea[MM_X(i) + MM_Y(j)*pBoard->width])
		BLOCK->pbBlockArea[i + j * BLOCK->size] = FALSE;
	BLOCK->pbBlockArea[x + y * BLOCK->size] = TRUE;
	// 因此rotate方法不需要调用draw
	// 通过在棋盘上标识，代表有点被转到了这个点，因此旋转这个点时不能直接取消
	pBoard->pcrGameArea[MM_X(x) + MM_Y(y)*pBoard->width] = BLOCK->color;
	return true;
}
static bool IRotate(GameBoard *pBoard) {
	if (BLOCK->xBlockLeft < 0 | BLOCK->xBlockLeft > pBoard->width - 4)
		return false;
	if (BLOCK->pbBlockArea[1]) {
		for (int i = 0; i < 4; i++)
			if (pBoard->pcrGameArea[MM_X(i) + MM_Y(2)*pBoard->width])
				return false;
		for (int i = 0; i < 4; i++) {
			BLOCK->pbBlockArea[1 + (3 - i)* BLOCK->size] = 0;
			BLOCK->pbBlockArea[i + 2 * BLOCK->size] = 1;
		}
		return true;
	}
	else {
		for (int i = 0; i < 4; i++)
			if (pBoard->pcrGameArea[MM_X(1) + MM_Y(i)*pBoard->width])
				return false;
		for (int i = 0; i < 4; i++) {
			BLOCK->pbBlockArea[i + 2 * BLOCK->size] = 0;
			BLOCK->pbBlockArea[1 + i * BLOCK->size] = 1;
		}
		return true;
	}
}
bool BlockRotate(GameBoard *pBoard) {
	// 2*2 只有正方形，不旋转
	if (BLOCK->size == 2)
		return false;
	// 3*3 变换矩阵为[0,-1,0;1,0,0;0,2,1]
	else if (BLOCK->size == 3) {
		if (Blockvisitor(pBoard, canPointRotate33)) {
			Blockvisitor(pBoard, clearBlock);
			Blockvisitor(pBoard, PointRotate33);
			return true;
		}
		else return false;
	}
	// 竖条单独旋转
	else if (BLOCK->size == 4) {
		Blockvisitor(pBoard, clearBlock);
		IRotate(pBoard);
		Blockvisitor(pBoard, drawBlock);
		return true;
	}
	else return false;
}

static bool canBlockLeft(int x, int y, GameBoard *pBoard) {
	if (MM_X(x) <= 0)
		return false;
	// 如果左侧有不是自身的方块，那么不能向左移动
	if (pBoard->pcrGameArea[MM_X(x) - 1 + MM_Y(y) * pBoard->width] && 
		// 记得对左上角第一个格判断，防止越界
		( MM_X(x) == BLOCK->xBlockLeft | (x - 1 + y * BLOCK->size) < 0 | 
			BLOCK->pbBlockArea[x - 1 + y * BLOCK->size] == 0))
		return false;
	else return true;
}
bool BlockLeft(GameBoard *pBoard) {
	if (!Blockvisitor(pBoard, canBlockLeft))
		return false;
	Blockvisitor(pBoard, clearBlock);
	BLOCK->xBlockLeft--;
	Blockvisitor(pBoard, drawBlock);
	return true;
}

static bool canBlockRight(int x, int y, GameBoard *pBoard) {
	if (MM_X(x) + 1 >= pBoard->width)
		return false;
	// 如果右侧有不是自身的方块，那么不能向右移动
	if (pBoard->pcrGameArea[MM_X(x) + 1 + MM_Y(y) * pBoard->width] &&
		(MM_X(x) >= BLOCK->xBlockLeft + BLOCK->size
			| x + 1 + y * BLOCK->size >= pow(BLOCK->size, 2)
			| BLOCK->pbBlockArea[x + 1 + y * BLOCK->size] == 0))
		return false;
	else return true;
}
bool BlockRight(GameBoard *pBoard) {
	if (!Blockvisitor(pBoard, canBlockRight))
		return false;
	Blockvisitor(pBoard, clearBlock);
	BLOCK->xBlockLeft++;
	Blockvisitor(pBoard, drawBlock);
	return true;
}

static void initBlock(GameBlock *pBlock, int size) {
	pBlock->size = size;
	pBlock->pbBlockArea = (bool*)malloc(sizeof(bool)*pow(size, 2));
	memset(pBlock->pbBlockArea, 0, sizeof(bool)*pow(size, 2));
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
void makeBlock(GameBlock **pBlock) {
#define BLOCK_AREA (*pBlock)->pbBlockArea
	if (*pBlock != NULL)
	{	
		if((*pBlock)->pbBlockArea != NULL)
			free(BLOCK_AREA);
		free(pBlock);
	}
	*pBlock = (GameBlock*)malloc(sizeof(GameBlock));
	switch (rand()%7) {
		// 左J
	case 1:
		initBlock(*pBlock, 3);
		BLOCK_AREA[0] = BLOCK_AREA[0 + 1 * 3] = BLOCK_AREA[1 + 1 * 3] = BLOCK_AREA[2 + 1 * 3] = 1;
		break;
		// 右J
	case 2:
		initBlock(*pBlock, 3);
		BLOCK_AREA[2] = BLOCK_AREA[0 + 1 * 3] = BLOCK_AREA[1 + 1 * 3] = BLOCK_AREA[2 + 1 * 3] = 1;
		break;
		// T
	case 3:
		initBlock(*pBlock, 3);
		BLOCK_AREA[1] = BLOCK_AREA[0 + 1 * 3] = BLOCK_AREA[1 + 1 * 3] = BLOCK_AREA[2 + 1 * 3] = 1;
		break;
		// 左Z
	case 4:
		initBlock(*pBlock, 3);
		BLOCK_AREA[1] = BLOCK_AREA[2] = BLOCK_AREA[0 + 1 * 3] = BLOCK_AREA[1 + 1 * 3] = 1;
		break;
		// 右Z
	case 5:
		initBlock(*pBlock, 3);
		BLOCK_AREA[0] = BLOCK_AREA[1] = BLOCK_AREA[1 + 1 * 3] = BLOCK_AREA[2 + 1 * 3] = 1;
		break;
		// I
	case 6:
		initBlock(*pBlock, 4);
		BLOCK_AREA[1] = BLOCK_AREA[1 + 1 * 4] = BLOCK_AREA[1 + 2 * 4] = BLOCK_AREA[1 + 3 * 4] = 1;
		break;
	default:
		initBlock(*pBlock, 2);
		BLOCK_AREA[0] = BLOCK_AREA[1] = BLOCK_AREA[2] = BLOCK_AREA[3] = 1;
		break;
	}
	(*pBlock)->color = initColor();
}
bool changeBlock(GameBoard* pBoard) {
	if (pBoard->pCurrentBlock != NULL)
		free(pBoard->pCurrentBlock->pbBlockArea);
	free(pBoard->pCurrentBlock);
	pBoard->pCurrentBlock = pBoard->pNextBlock;
	pBoard->pNextBlock = NULL;
	makeBlock(&pBoard->pNextBlock);
	pBoard->pCurrentBlock->yBlockTop = 0;
	pBoard->pCurrentBlock->xBlockLeft = pBoard->width/2-1;
	if (!Blockvisitor(pBoard, canDrawBlock)) {
		pBoard->bGameOver = true;
		return false;
	}
	Blockvisitor(pBoard, drawBlock);
	return true;
}
void makeBoard(GameBoard **pBoard, int width, int height) {
	if (*pBoard != NULL)
	{	
		if((*pBoard)->pcrGameArea != NULL)
			free((*pBoard)->pcrGameArea);
		if ((*pBoard)->pCurrentBlock != NULL) {
			if ((*pBoard)->pCurrentBlock->pbBlockArea != NULL)
				free((*pBoard)->pCurrentBlock->pbBlockArea);
			free((*pBoard)->pCurrentBlock);
		}
		if ((*pBoard)->pNextBlock != NULL) {
			if ((*pBoard)->pNextBlock->pbBlockArea != NULL)
				free((*pBoard)->pNextBlock->pbBlockArea);
			free((*pBoard)->pNextBlock);
		}
		free(*pBoard);
	}
	(*pBoard) = (GameBoard*)malloc(sizeof(GameBoard));
	(*pBoard)->height = height;
	(*pBoard)->width = width;
	(*pBoard)->pcrGameArea = (COLORREF*)malloc(sizeof(COLORREF)*height*width);
	memset((*pBoard)->pcrGameArea, 0, sizeof(COLORREF)*height*width);
	(*pBoard)->bGameOver = false;
	(*pBoard)->score = 0;
	(*pBoard)->pCurrentBlock = NULL;
	(*pBoard)->pNextBlock = NULL;
	makeBlock(&(*pBoard)->pNextBlock);
	changeBlock((*pBoard));
}