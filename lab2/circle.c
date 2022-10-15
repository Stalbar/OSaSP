#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	
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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "CircleWriterClass";
	wcex.hIconSm = wcex.hIcon;
	
	RegisterClassEx(&wcex);
	
	hWnd = CreateWindow("CircleWriterClass", "Circle Writer", WS_OVERLAPPEDWINDOW, 0, 0, 500, 500, NULL, NULL, hInstance, NULL);
	
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
	switch (Message)
	{
		
		case WM_PAINT:
			int radius = 20;
			char* word = "lorem";
			HDC hDC;
			PAINTSTRUCT ps;
			hDC = BeginPaint(hWnd, &ps);
			SetBkMode(hDC, TRANSPARENT);
			LOGFONT lf;
			HFONT hFont, hOldFont;
			lf.lfHeight = 40;
			lf.lfWidth = 0;
			lf.lfEscapement = 0;
			lf.lfOrientation = 2700;
			lf.lfWeight = FW_SEMIBOLD;
			lf.lfItalic = 0;
			lf.lfUnderline = 0;
			lf.lfStrikeOut = 0;
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfOutPrecision = 0;
			lf.lfClipPrecision = 0;
			lf.lfQuality = 0;
			lf.lfPitchAndFamily = 0;
			strcpy(lf.lfFaceName, "Arial");
			int i = 0;
			int delta = 3600 / 5;
			while (*word)
			{
				lf.lfEscapement = delta * i;
				hFont = CreateFontIndirect(&lf);
				hOldFont = SelectObject(hDC, hFont);
				TextOut(hDC, 100 + radius * cos(3600/5 - lf.lfEscapement) , 100 + radius * sin(3600/5 - lf.lfEscapement), word, 1);
				i++;
				word++;
				SelectObject(hDC, hOldFont);
				DeleteObject(hFont);
			}
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}