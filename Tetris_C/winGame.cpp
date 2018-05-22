// ����windows������ƿγ̵���ϰ
#include "stdafx.h"
#include "Tetris_C.h"
#include <tchar.h>

#define WIDTH 10
#define HEIGHT 16
#define BASE_DIFF 550

// �������ڹ��̴�������ԭ��
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain��Windows�������
/*
#define WINAPI __stdcall
"sz"���������β���ַ�����
HINSTANCE: ʵ������� �����һ����ֵ�����ݴ�ֵ������ϵͳ�ɻ�ö�Ӧ�Ĵ��ڡ��ļ����豸�ȵ�ָ��
iCmdShow: ָʾ�˳�������ԣ���С����ȫ������������ʾ
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	// TCHAR ��windows ά���ĺ꣬���Ա����Unicode(���ַ�) ���� ASCII�汾�ĺ����ͱ�������
	static TCHAR szAppName[] = TEXT("Tetris");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc; // ������Ӧ�Ĵ��ڹ���
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; // ����ҪԤ������ռ�
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ���ô��ڵĻ�ˢ
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName; //  �����������

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0;
	}

	// ִ����˺�����windows�ѷ�����һ���ڴ��������洰�ڵ���Ϣ
	hwnd = CreateWindow(szAppName, TEXT("Tetris"),
		WS_OVERLAPPEDWINDOW, // ���ڷ��
		200, 50, 750, 800, // ��ʼx/y���꣬x/y����ߴ�
		NULL, NULL, hInstance, NULL); // �����ھ�������ڲ˵����������ʵ���������������
									  // ��������ʾ����Ļ��
	ShowWindow(hwnd, iCmdShow);
	// �򴰿ڹ��̷���WM_PAINT��Ϣ��ʹ�ͻ����ػ�
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		// ��msg����windows���м�����Ϣת��
		TranslateMessage(&msg);
		// ��msg����windows��������Ӧ�Ĵ��ڹ��̴���
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
// ���ڶ�̬������Ҫ�Ĵ��ڴ�С
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
		// ������Ϸ����
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
		// �����ָ���
		MoveToEx(hdc, xSlot*BLOCK_SIZE, 0, NULL);
		LineTo(hdc, xSlot*BLOCK_SIZE, cyClient);
		// ������һ������
		int xoffset = BLOCK_SIZE * (xSlot + 1);
		// �����ر���
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
		// ��������
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
