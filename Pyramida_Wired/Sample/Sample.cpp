#include <windows.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979f
#define TWO_PI (PI * 2.0f)

HWND g_hWnd;

struct vector3
{
	float x, y, z;
};

enum { A, B, C, D, E };

vector3 g_VertBuff[8] = {
	-4.0f,	-4.0f,	-4.0f,		//A
	 4.0f,	-4.0f,	-4.0f,		//B
	-4.0f,	-4.0f,	 4.0f,		//C 
	 4.0f,	-4.0f,	 4.0f,		//D 
	 0.0f,	 4.0f,	 0.0f };	//E Top


vector3 g_VertBuffTransformed[8];

unsigned int g_IndexBuff[16] = {
		A, E,
		E, B,
		B, A,

		C, E,
		E, D,
		D, C,

		A, C,
		D, B };


typedef float matrix4x4[4][4];

vector3 Vec3_Mat4x4_Mul(vector3& VecIn, matrix4x4 MatIn)
{
	vector3 VecOut;

	VecOut.x =	VecIn.x * MatIn[0][0] +
				VecIn.y * MatIn[1][0] +
				VecIn.z * MatIn[2][0] +
						  MatIn[3][0];

	VecOut.y =	VecIn.x * MatIn[0][1] +
				VecIn.y * MatIn[1][1] +
				VecIn.z * MatIn[2][1] +
						  MatIn[3][1];

	VecOut.z =	VecIn.x * MatIn[0][2] +
				VecIn.y * MatIn[1][2] +
				VecIn.z * MatIn[2][2] +
						  MatIn[3][2];

	return VecOut;
}

float Vec3_Dot(vector3& VecIn1, vector3& VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

vector3 Vec3_Normalize(vector3& VecIn1)
{
	float Len = sqrtf((VecIn1.x * VecIn1.x) + (VecIn1.y * VecIn1.y) + (VecIn1.z * VecIn1.z));

	vector3 t = { VecIn1.x / Len, VecIn1.y / Len, VecIn1.z / Len };

	return t;
}

vector3 Vec3_Cross(vector3& VecIn1, vector3& VecIn2)
{
	vector3 VecOut = { VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y,
		VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z,
		VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x };

	return VecOut;
}

void Draw_Pyramid()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static float Angle = 2.576107f;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 };

	char buff[32];
	sprintf_s(buff, 32, "%f\n", Angle);
	OutputDebugString(buff);

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


	for (int i = 0; i < 5; i++)
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatWorld);
		Vec = Vec3_Mat4x4_Mul(Vec, MatView);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ((float)Rc.right / Rc.bottom);

		Vec.x = Vec.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec.y = -Vec.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}

	HDC hDC = GetDC(g_hWnd);

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	FillRect(hDC, &Rc, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	for (int i = 0; i < 8; i++)
	{
		vector3 Vec1 = g_VertBuffTransformed[g_IndexBuff[i * 2 + 0]];
		vector3 Vec2 = g_VertBuffTransformed[g_IndexBuff[i * 2 + 1]];

		MoveToEx(hDC, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(hDC, (int)Vec2.x, (int)Vec2.y);
	}

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	ReleaseDC(g_hWnd, hDC);

	Sleep(25);
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

		Draw_Pyramid();
	}
	
	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}