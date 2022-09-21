#include <windows.h>

#define ID_MOVEMENT_TIMER 1001
#define ID_AWAY_FROM_KEYBOARD_TIMER 1002

typedef enum Direction {
	UP = 1,
	DOWN = 2,
	LEFT = 4,
	RIGHT = 8
} Direction;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void FixCoordinates(int*, int*);
void ChangeDirection(int*, int*, Direction*);

int width, height;
int speed = 5;
int xPos;
int yPos;
int size = 150;
Direction direction = UP | LEFT;

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
	wcex.lpszClassName = "SpriteMovementClass";
	wcex.hIconSm = wcex.hIcon;
	
	RegisterClassEx(&wcex);
	
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	
	xPos = width / 2;
	yPos = height / 2;
	
	hWnd = CreateWindow("SpriteMovementClass", "Sprite movement", WS_OVERLAPPEDWINDOW, 0, 0, width, height, NULL, NULL, hInstance, NULL);
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	SetTimer(hWnd, ID_AWAY_FROM_KEYBOARD_TIMER, 2000, NULL);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	HBRUSH hBrush, oldBrush;
	int wheelDelta = 0;
	RECT rect;
	RECT rectToRedraw;
	POINT upMouseCoordinates;
    switch (Message)
    {
		case WM_PAINT:
			GetWindowRect(hWnd, &rect);
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
			hDC = BeginPaint(hWnd, &ps);
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			oldBrush = SelectObject(hDC, hBrush);
			Ellipse(hDC, xPos, yPos, xPos + size, yPos + size);
			SelectObject(hDC, oldBrush);
			DeleteObject(hBrush);
			EndPaint(hWnd, &ps);
			return 0;
		case WM_TIMER:
			switch (wParam)
			{
				case ID_AWAY_FROM_KEYBOARD_TIMER:
					KillTimer(hWnd, ID_AWAY_FROM_KEYBOARD_TIMER);
					SetTimer(hWnd, ID_MOVEMENT_TIMER, 70, NULL);
					break;
				case ID_MOVEMENT_TIMER:
					if (direction & UP)
						yPos -= speed;
					if (direction & DOWN)
						yPos += speed;
					if (direction & LEFT)
						xPos -= speed;
					if (direction & RIGHT)
						xPos += speed;
					rectToRedraw.top = yPos - 20;
					rectToRedraw.left = xPos - 20;
					rectToRedraw.right = xPos + size + 20;
					rectToRedraw.bottom = yPos + size + 20;
					ChangeDirection(&xPos, &yPos, &direction);
					InvalidateRect(hWnd, &rectToRedraw, TRUE);
					break;
			}
			break;
		case WM_KEYDOWN:
			KillTimer(hWnd, ID_MOVEMENT_TIMER);
			switch (wParam)
			{
				case 0x41:
				case VK_LEFT:
					xPos -= speed;
					direction |= LEFT;
					direction &= ~RIGHT;
					break;
				case 0x44:
				case VK_RIGHT:
					xPos += speed;
					direction |= RIGHT;
					direction &= ~LEFT;
					break;
				case 0x57:
				case VK_UP:
					yPos -= speed;
					direction |= UP;
					direction &= ~DOWN;
					break;
				case 0x53:
				case VK_DOWN:
					yPos += speed;
					direction |= DOWN;
					direction &= ~UP;
					break;
			}
			FixCoordinates(&xPos, &yPos);
			InvalidateRect(hWnd, NULL, TRUE);
			SetTimer(hWnd, ID_AWAY_FROM_KEYBOARD_TIMER, 500, NULL);
			break;
		case WM_MOUSEWHEEL:
			KillTimer(hWnd, ID_MOVEMENT_TIMER);
			wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (wheelDelta > 0)
			{
				if (LOWORD(wParam) == MK_SHIFT)
				{
					xPos += speed;
					direction |= RIGHT;
					direction &= ~LEFT;
				}
				else
				{
					yPos -= speed;
					direction |= UP;
					direction &= ~DOWN;
				}
			}
			else
			{
				if (LOWORD(wParam) == MK_SHIFT)
				{
					xPos -= speed;
					direction |= LEFT;
					direction &= ~RIGHT;
				}
				else
				{
					yPos += speed;
					direction |= DOWN;
					direction &= ~UP;
				}
			}
			FixCoordinates(&xPos, &yPos);
			InvalidateRect(hWnd, NULL, TRUE);
			SetTimer(hWnd, ID_AWAY_FROM_KEYBOARD_TIMER, 500, NULL);
			break;
		case WM_LBUTTONDOWN:
			KillTimer(hWnd, ID_AWAY_FROM_KEYBOARD_TIMER);
			KillTimer(hWnd, ID_MOVEMENT_TIMER);
			break;
		case WM_LBUTTONUP:
			upMouseCoordinates.x = LOWORD(lParam);
			upMouseCoordinates.y = HIWORD(lParam);
			xPos = upMouseCoordinates.x;
			yPos = upMouseCoordinates.y;
			FixCoordinates(&xPos, &yPos);
			InvalidateRect(hWnd, NULL, TRUE);
			SetTimer(hWnd, ID_AWAY_FROM_KEYBOARD_TIMER, 500, NULL);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
    }
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

void FixCoordinates(int* xCoord, int* yCoord)
{
	if (*xCoord < 0)
		*xCoord = 0;
	if (*xCoord + size > width)
		*xCoord = width - size;
	if (*yCoord < 0)
		*yCoord = 0;
	if (*yCoord + size > height - 20)
		*yCoord = height - size - 20;
}

void ChangeDirection(int* xCoord, int* yCoord, Direction* direction)
{
	if (*xCoord < 0 && *yCoord < 0)
	{
		*xCoord = 0;
		*yCoord = 0;
		*direction &= ~LEFT & ~UP;
		*direction |= RIGHT | DOWN;
		return;
	}
	if (*xCoord < 0 && *yCoord + size > height - 20)
	{
		*xCoord = 0;
		*yCoord = height - size - 20;
		*direction &= ~DOWN & ~LEFT;
		*direction |= UP | RIGHT;
		return;
	}
	if (*xCoord + size > width && *yCoord + size > height - 20)
	{
		*xCoord = width - size;
		*yCoord = height - size - 20;
		*direction &= ~DOWN & ~RIGHT;
		*direction |= UP | LEFT;
		return;
	}
	if (*xCoord + size > width && *yCoord < 0)
	{
		*xCoord = width - size;
		*yCoord = 0;
		*direction &= ~RIGHT & ~UP;
		*direction |= DOWN | LEFT;
		return;
	}
	if (*xCoord < 0)
	{
		*xCoord = 0;
		*direction &= ~LEFT;
		*direction |=  RIGHT;
		return;
	}
	if (*xCoord + size > width)
	{
		*xCoord = width - size;
		*direction &= ~RIGHT;
		*direction |= LEFT;
		return;
	}
	if (*yCoord < 0)
	{
		*yCoord = 0;
		*direction &= ~UP;
		*direction |= DOWN;
		return;
	}
	if (*yCoord + size > height - 20)
	{
		*yCoord = height - size - 20;
		*direction &= ~DOWN;
		*direction |= UP;
		return;
	}
}