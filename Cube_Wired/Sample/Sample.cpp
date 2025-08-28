#include <windows.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

HWND g_hWnd;

struct vector3
{
	float x,y,z;
};

enum { A, B, C, D, E, F, G, H };

vector3 g_VertBuff[8] = {
	-4.0, -4.0, -4.0,	//A
	 4.0, -4.0, -4.0,	//B
	-4.0,  4.0, -4.0,	//C
	 4.0,  4.0, -4.0,	//D

 	-4.0, -4.0,  4.0,	//E
	 4.0, -4.0,  4.0,	//F
	-4.0,  4.0,  4.0,	//G
	 4.0,  4.0,  4.0 };	//H

vector3 g_VertBuffTransformed[8];

/*
	CUBE VERTICES

	FONT SIDE	BACK SIDE
	C - D		G - H
	|   |		|   |
	A - B		E - F
*/

unsigned int g_IndexBuff[24] = {
	//LINES CONNECT FRONT SIDE VERTICES
	A, B,
	B, D,
	D, C,
	C, A,

	//LINES CONNECT BACK SIDE VERTICES
	E, F,
	F, H,
	H, G,
	G, E,

	//LINES CONNECT FRONT AND BACK SIDE
	A, E,
	B, F,
	D, H,
	C, G };

typedef float matrix4x4[4][4];

//контекст окна приложения
HDC g_hDC;
//размеры окна приложения
UINT g_ViewWidth;
UINT g_ViewHeight;
//наш задний буфер
HDC g_hBackBuffer;
HBITMAP g_hBitmap;
HBITMAP g_hOldBitmap;
//кисть для очистки заднего буфера (окна приложения)
HBRUSH g_hBrush;
//область для очистки заднего буфера (окна приложения)
RECT g_Rc;

void Create_BackBuffer();
void Clear_BackBuffer();
void Present_BackBuffer();
void Delete_BackBuffer();

vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
{
	vector3 VecOut;

	VecOut.x =	VecIn.x * MatIn[0][0] +
				VecIn.y * MatIn[1][0] +
				VecIn.z * MatIn[2][0] +
					      MatIn[3][0];

	VecOut.y =  VecIn.x * MatIn[0][1] +
				VecIn.y * MatIn[1][1] +
				VecIn.z * MatIn[2][1] +
					      MatIn[3][1];

	VecOut.z =  VecIn.x * MatIn[0][2] +
				VecIn.y * MatIn[1][2] +
				VecIn.z * MatIn[2][2] +
					      MatIn[3][2];

	return VecOut;
}

float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

vector3 Vec3_Normalize(vector3 &VecIn1)
{
	float Len = sqrtf( (VecIn1.x * VecIn1.x) + (VecIn1.y * VecIn1.y) + (VecIn1.z * VecIn1.z) );
	
	vector3 t = { VecIn1.x / Len, VecIn1.y / Len, VecIn1.z / Len };

	return t;
}

vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut = { VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y,
		VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z,
		VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x };

	return VecOut;
}

void Scene_Draw()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static float Angle = 2.576107f;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 };

	/*
	Angle = Angle + PI / 100.0f;
	if(Angle > PI2)
		Angle = 0.0f;
	*/

	vector3 VecModelPos = { 0.0, 0.0, 0.0 };
	
	//MATRIX WORLD
	matrix4x4 MatWorld = {
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		VecModelPos.x, VecModelPos.y, VecModelPos.z, 1.0f }; 

	//MATRIX VIEW CALCULATION
	vector3 VecCamRight = { 1.0f, 0.0f, 0.0 };
	vector3 VecCamUp = { 0.0f, 1.0f, 0.0f };
	vector3 VecCamPos = { 0.0f, 6.0f, -12.0f };
	vector3 VecCamLook = { -1.0f * VecCamPos.x, -1.0f * VecCamPos.y, -1.0f * VecCamPos.z };

	VecCamLook = Vec3_Normalize(VecCamLook);

	VecCamUp = Vec3_Cross(VecCamLook, VecCamRight);
	VecCamUp = Vec3_Normalize(VecCamUp);
	VecCamRight = Vec3_Cross(VecCamUp, VecCamLook);
	VecCamRight = Vec3_Normalize(VecCamRight);

	float Px = -Vec3_Dot(VecCamPos, VecCamRight);
	float Py = -Vec3_Dot(VecCamPos, VecCamUp);
	float Pz = -Vec3_Dot(VecCamPos, VecCamLook);

	matrix4x4 MatView = {
		VecCamRight.x,		VecCamUp.x,	VecCamLook.x,		0.0f,
		VecCamRight.y,		VecCamUp.y,	VecCamLook.y,		0.0f,
		VecCamRight.z,		VecCamUp.z,	VecCamLook.z,		0.0f,
		Px,				Py,			Pz,				1.0f };

	for ( int i = 0; i < 8; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatWorld);
		Vec = Vec3_Mat4x4_Mul(Vec, MatView);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ( (float) Rc.right / Rc.bottom );

		Vec.x = Vec.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec.y =-Vec.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}

	HPEN hPen = CreatePen(PS_SOLID, 3, RGB( 0, 0, 0 ) );
	HPEN hOldPen = (HPEN) SelectObject(g_hBackBuffer, hPen);

	Clear_BackBuffer();

	for ( int i = 0; i < 12; i++ )
	{
		vector3 Vec1 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 0] ];
		vector3 Vec2 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 1 ] ];

		MoveToEx(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(g_hBackBuffer, (int)Vec2.x, (int)Vec2.y);
	}

	SelectObject(g_hBackBuffer, hOldPen);
	DeleteObject(hPen);

	Present_BackBuffer();

	Sleep(25);
}

void Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(g_hWnd, &g_Rc);

	g_ViewWidth = g_Rc.right;
	g_ViewHeight = g_Rc.bottom;

	g_hDC = GetDC(g_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	g_hBrush = CreateSolidBrush(RGB(255, 255, 255));

	//наш задний буфер
	g_hBackBuffer = CreateCompatibleDC(g_hDC);
	g_hBitmap = CreateCompatibleBitmap(g_hDC, g_ViewWidth, g_ViewHeight);
	g_hOldBitmap = (HBITMAP)SelectObject(g_hBackBuffer, g_hBitmap);
}

void Clear_BackBuffer()
{
	//очистка окна приложения
	FillRect(g_hBackBuffer, &g_Rc, g_hBrush);
}

void Present_BackBuffer()
{
	//выводим задний буфер на экран приложения
	BitBlt(g_hDC, 0, 0, g_ViewWidth, g_ViewHeight, g_hBackBuffer, 0, 0, SRCCOPY);
}

void Delete_BackBuffer()
{
	//уничтожение заднего буфера
	SelectObject(g_hBackBuffer, g_hOldBitmap);
	DeleteObject(g_hBitmap);

	DeleteObject(g_hBrush);

	ReleaseDC(g_hWnd, g_hDC);
	DeleteDC(g_hBackBuffer);
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
	
}

int PASCAL WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASS wcl = { 0, WndProc, 0, 0, hInstance, 0, 0, 0, 0, "Sample"};

	if(!RegisterClass(&wcl))
		return 0;
    
	g_hWnd = CreateWindow("Sample", "Sample 3D Application - Ed Kurlyak", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                             NULL, NULL, hInstance, NULL);

	if(!g_hWnd)
		return 0;
    
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	Create_BackBuffer();

	MSG msg;

	while(true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message ==	WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;

		Scene_Draw();
	}

	Delete_BackBuffer();
	
	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}