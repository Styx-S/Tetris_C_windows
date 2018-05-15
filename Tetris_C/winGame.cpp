// 用于windows程序设计课程的练习
#include "stdafx.h"
#include "Tetris_C.h"

#define WIDTH 10
#define HEIGHT 16

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
	static TCHAR szAppName[] = TEXT("WindowsProgramingLearning");
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
	hwnd = CreateWindow(szAppName, TEXT("Learn Windows Programing"),
		WS_OVERLAPPEDWINDOW, // 窗口风格
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // 初始x/y坐标，x/y方向尺寸
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
typedef struct {
	int xLeft, yTop;
	int xRight, yBottom;
} GameRect;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient, xSlot = 10, ySlot = 16;
	static GameBoard *pGame;
	HDC hdc;
	HBRUSH hBrush;
	PAINTSTRUCT ps;
	int i, j;
	switch (message)
	{
	case WM_CREATE:
		srand(time(NULL));
		makeBoard(&pGame, xSlot, ySlot);
		SetTimer(hwnd, 1, 550, NULL);
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
			BlockDown(pGame);
			InvalidateRect(hwnd, NULL, TRUE);
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
		for (int i = 0; i < xSlot; i++) {
			for (int j = 0; j < ySlot; j++) {
				if (pGame->pcrGameArea[i + j * xSlot]) {
					hBrush = CreateSolidBrush(pGame->pcrGameArea[i + j * xSlot]);
					SelectObject(hdc, hBrush);
					Rectangle(hdc, i*cxClient / xSlot, j*cyClient / ySlot,
						(i + 1)*cxClient / xSlot, (j + 1)*cyClient / ySlot);
					DeleteObject(hBrush);
				}
			}
		}
		EndPaint(hwnd, &ps);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
