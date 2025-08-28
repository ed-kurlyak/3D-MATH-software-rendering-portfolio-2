#include <math.h>
#include <stdio.h>
#include <windows.h>

#define PI 3.14159265358979f

HWND g_hWnd = NULL;

struct Vec2
{
	float x;
	float y;
};

float Fun(float x, float y)
{
	return cosf(sqrtf(x * x + y * y));
}

Vec2 Transform(float x1, float y1, float z1)
{
	Vec2 VecOut;

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	float Theta = 0.5f;
	float Phi = 0.6f;

	float z = Fun((float)x1, (float)y1);

	float xx = x1 * sinf(Theta) + y1 * cosf(Theta);
	float yy = x1 * cosf(Theta) * cosf(Phi) - y1 * sinf(Theta) * cosf(Phi) + z * sinf(Phi);
	float zz = x1 * cosf(Theta) * sinf(Phi) - y1 * sinf(Theta) * sinf(Phi) - z * cosf(Phi);

	zz = zz + 60.0f;

	float D = 10.0f;

	xx = D * xx / zz;
	yy = D * yy / zz;

	VecOut.x = xx * Rc.bottom / 2.0f + Rc.right / 2.0f;
	VecOut.y = -yy * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

	return VecOut;

}

void Draw_Surface()
{
	HDC hdc = GetDC(g_hWnd);

	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	SelectObject(hdc, hPen);

	float xMin = -5.0f;
	float xMax = 5.0f;
	float yMin = -5.0f;
	float yMax = 5.0f;

	int xCount = 20;
	int yCount = 20;

	float xStep = (xMax - xMin) / xCount; // 0.5
	float yStep = (yMax - yMin) / yCount;

	// вертикальные линии (X фиксирован, Y меняется)
	for (int i = 0; i <= xCount; i++)
	{
		float x = xMin + i * xStep;

		for (int j = 0; j < yCount; j++)
		{
			float y1 = yMin + j * yStep;
			float y2 = yMin + (j + 1) * yStep;

			Vec2 Vec1 = Transform(x, y1, 0);
			Vec2 Vec2 = Transform(x, y2, 0);

			MoveToEx(hdc, (int)Vec1.x, (int)Vec1.y, NULL);
			LineTo(hdc, (int)Vec2.x, (int)Vec2.y);
		}
	}

	// горизонтальные линии (Y фиксирован, X меняется)
	for (int j = 0; j <= yCount; j++)
	{
		float y = yMin + j * yStep;

		for (int i = 0; i < xCount; i++)
		{
			float x1 = xMin + i * xStep;
			float x2 = xMin + (i + 1) * xStep;

			Vec2 Vec1 = Transform(x1, y, 0);
			Vec2 Vec2 = Transform(x2, y, 0);

			MoveToEx(hdc, (int)Vec1.x, (int)Vec1.y, NULL);
			LineTo(hdc, (int)Vec2.x, (int)Vec2.y);
		}
	}

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);


		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "SurfaceWindowClass";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	g_hWnd =
		CreateWindow("SurfaceWindowClass", "Sample 3D Application - Ed Kurlyak",
					 WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
					 CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

	if (g_hWnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
				   MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	Draw_Surface();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;
	}

	return (int)msg.wParam;
}