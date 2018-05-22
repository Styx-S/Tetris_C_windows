// 用于windows程序设计课程的练习
#include "stdafx.h"
#include "Tetris_C.h"
#include <tchar.h>

#define WIDTH 10
#define HEIGHT 16
#define BASE_DIFF 550

// 声明窗口过程处理函数的原型
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain：Windows程序入口
/*
#define WINAPI __stdcall
"sz"：“以零结尾的字符串”
HINSTANCE: 实例句柄。 句柄：一个数值，传递此值给操作系统可获得对应的窗口、文件、设备等的指针
iCmdShow: 指示此程序最初以（最小化、全屏、正常）显示
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	// TCHAR 是windows 维护的宏，可以编译出Unicode(宽字符) 或者 ASCII版本的函数和变量操作
	static TCHAR szAppName[] = TEXT("Tetris");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc; // 关联相应的窗口过程
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; // 不需要预留额外空间
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 设置窗口的画刷
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName; //  窗口类的名字

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	// 执行完此函数，windows已分配了一块内存用来保存窗口的信息
	hwnd = CreateWindow(szAppName, TEXT("Tetris"),
		WS_OVERLAPPEDWINDOW, // 窗口风格
		200, 50, 750, 800, // 初始x/y坐标，x/y方向尺寸
		NULL, NULL, hInstance, NULL); // 父窗口句柄、窗口菜单句柄、程序实例句柄、创建参数
									  // 将窗口显示在屏幕上
	ShowWindow(hwnd, iCmdShow);
	// 向窗口过程发送WM_PAINT消息，使客户区重绘
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		// 将msg发给windows进行键盘消息转换
		TranslateMessage(&msg);
		// 将msg发给windows，交给对应的窗口过程处理
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
// 用于动态计算需要的窗口大小
static bool caculateSubRect(int width, int height, int subWidth, int subHeight, int *px, int *py) {
	if (width < subWidth | height < subHeight)
		return false;
	*px = (width - subWidth) / 2;
	*py = (height - subHeight) / 2;
	return true;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient, xSlot = 10, ySlot = 16;
	static double BLOCK_SIZE;
	static TCHAR szBuffer[64];
	static GameBoard *pGame;
	HDC hdc;
	HBRUSH hBrush;
	PAINTSTRUCT ps;
	HFONT hf;
	int i, j, xoffset, yoffset;
	switch (message)
	{
	case WM_CREATE:

		srand(time(NULL));
		makeBoard(&pGame, xSlot, ySlot);
		SetTimer(hwnd, 1, BASE_DIFF, NULL);
		return 0;
	case WM_TIMER:
		SendMessage(hwnd, WM_KEYDOWN, VK_DOWN, NULL);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		BLOCK_SIZE = 1.0*cyClient / ySlot;
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_KEYDOWN:
		if (pGame->bGameOver)
			return 0;
		switch (wParam)
		{
		case VK_UP:
			if (BlockRotate(pGame))
				InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case VK_DOWN:
			if (BlockDown(pGame)) {
				InvalidateRect(hwnd, NULL, TRUE);
				int interval = BASE_DIFF * (1.0 - pGame->score*1.0 / 20);
				SetTimer(hwnd, 1, interval, NULL);
			}
			return 0;
		case VK_LEFT:
			if (BlockLeft(pGame))
				InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case VK_RIGHT:
			if (BlockRight(pGame))
				InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		// 画出游戏界面
		for (int i = 0; i < xSlot; i++) {
			for (int j = 0; j < ySlot; j++) {
				if (pGame->pcrGameArea[i + j * xSlot]) {
					hBrush = CreateSolidBrush(pGame->pcrGameArea[i + j * xSlot]);
					SelectObject(hdc, hBrush);
					Rectangle(hdc, i*BLOCK_SIZE, j*BLOCK_SIZE, (i + 1)*BLOCK_SIZE, (j + 1)*BLOCK_SIZE);
					DeleteObject(hBrush);
				}
			}
		}
		// 画出分割线
		MoveToEx(hdc, xSlot*BLOCK_SIZE, 0, NULL);
		LineTo(hdc, xSlot*BLOCK_SIZE, cyClient);
		// 画出下一个方块
		int xoffset = BLOCK_SIZE * (xSlot + 1);
		// 竖条特别处理
		if (pGame->pNextBlock->size == 4)
			yoffset = (ySlot)/8;
		else yoffset = (ySlot / 4);
		for (int i = 0; i < pGame->pNextBlock->size; i++){
			for (int j = 0; j < pGame->pNextBlock->size; j++) {
				if (pGame->pNextBlock->pbBlockArea[i + j * pGame->pNextBlock->size]) {
					hBrush = CreateSolidBrush(pGame->pNextBlock->color);
					SelectObject(hdc, hBrush);
					Rectangle(hdc, xoffset + i * BLOCK_SIZE, yoffset*BLOCK_SIZE + j * BLOCK_SIZE,
						xoffset + (i + 1)*BLOCK_SIZE, yoffset*BLOCK_SIZE + (j + 1)*BLOCK_SIZE);
					DeleteObject(hBrush);
				}
			}
		}
		// 画出分数
		hf = CreateFont(20, 0, 0, 0, 400, 0, 0, 0, OEM_CHARSET,
			0, 0, 0, FIXED_PITCH, NULL);
		TextOut(hdc, xoffset + 1 + BLOCK_SIZE, (ySlot) / 8 * 3 * BLOCK_SIZE, TEXT("Next"), _tcslen(TEXT("Next")));
		SelectObject(hdc, hf);
		wsprintf(szBuffer, TEXT("Score: %d"), pGame->score);
		TextOut(hdc, xoffset+1+BLOCK_SIZE, (ySlot)/2*BLOCK_SIZE, szBuffer, _tcslen(szBuffer));
		DeleteObject(hf);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
