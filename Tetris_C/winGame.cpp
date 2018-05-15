// ����windows������ƿγ̵���ϰ
#include "stdafx.h"
#include "Tetris_C.h"

#define WIDTH 10
#define HEIGHT 16

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
	static TCHAR szAppName[] = TEXT("WindowsProgramingLearning");
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
	hwnd = CreateWindow(szAppName, TEXT("Learn Windows Programing"),
		WS_OVERLAPPEDWINDOW, // ���ڷ��
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // ��ʼx/y���꣬x/y����ߴ�
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
