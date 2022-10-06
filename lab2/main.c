#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define ROW_COUNT 5
#define COL_COUNT 5
#define BUFFER_SIZE 1024 * 1024
#define BETWEEN_PIXELS

typedef struct Cell
{
	RECT rect;
	char buffer[BUFFER_SIZE];
} MatrixCell;

MatrixCell matrix[ROW_COUNT][COL_COUNT];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));
	
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "RectangleWriterClass";
	wcex.hIconSm = wcex.hIcon;
	
	RegisterClassEx(&wcex);
	
	hWnd = CreateWindow("RectangleWriterClass", "Rectangle Writer", WS_OVERLAPPEDWINDOW, 0, 0, 500, 500, NULL, NULL, hInstance, NULL);
	
	HANDLE hFile;
	char readBuffer[BUFFER_SIZE];
	DWORD dwBytesRead = 0;
	
	hFile = CreateFile("textFile.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(hFile, readBuffer, BUFFER_SIZE - 1, &dwBytesRead, NULL);
	readBuffer[BUFFER_SIZE] = '\0';
	CloseHandle(hFile);
	
	for (int i = 0; i < ROW_COUNT; i++)
	{
		for (int j = 0; j < COL_COUNT; j++)
		{
			int randomLength = rand() % (dwBytesRead);
			CopyMemory(matrix[i][j].buffer, readBuffer, randomLength - 1);
			matrix[i][j].buffer[randomLength] = '\0';
		}
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int windowWidth = 0;
	PAINTSTRUCT ps;
	RECT rect;
	int rectWidth;
	HDC hDC;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	switch (Message)
	{
		case WM_PAINT:
			GetClientRect(hWnd, &rect);
			windowWidth = rect.right - rect.left;
			rectWidth = windowWidth / COL_COUNT;
			hDC = BeginPaint(hWnd, &ps);
			for (int i = 0; i < ROW_COUNT; i++)
			{
				for (int j = 0; j < COL_COUNT; j++)
				{
					hBrush = CreateSolidBrush(RGB(0, 0, 0));
					hOldBrush = SelectObject(hDC, hBrush);
					SetTextColor(hDC, RGB(0, 170, 0));
					matrix[i][j].rect.left = rectWidth * j;
					matrix[i][j].rect.right = matrix[i][j].rect.left + rectWidth;
					matrix[i][j].rect.top = 0;
					matrix[i][j].rect.bottom = 0;
					DrawText(hDC, matrix[i][j].buffer, -1, &(matrix[i][j].rect), DT_CALCRECT | DT_WORDBREAK | DT_LEFT | DT_EDITCONTROL | DT_WORD_ELLIPSIS);
					if (i > 0)
					{
						matrix[i][j].rect.top += matrix[i - 1][j].rect.bottom;
						matrix[i][j].rect.bottom += matrix[i - 1][j].rect.bottom;
					}
					matrix[i][j].rect.left = rectWidth * j;
					matrix[i][j].rect.right = matrix[i][j].rect.left + rectWidth;
					hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
					hOldPen = SelectObject(hDC, hPen);
					Rectangle(hDC, matrix[i][j].rect.left, matrix[i][j].rect.top, matrix[i][j].rect.right,  matrix[i][j].rect.bottom);
					SelectObject(hDC, hOldPen);
					SelectObject(hDC, hOldBrush);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					SetBkMode(hDC, TRANSPARENT);
					DrawText(hDC, matrix[i][j].buffer, -1, &(matrix[i][j].rect), DT_WORDBREAK | DT_LEFT | DT_EDITCONTROL);
				}
			}
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}