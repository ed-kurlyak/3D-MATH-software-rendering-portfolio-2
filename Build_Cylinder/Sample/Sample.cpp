#include <windows.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

struct vector3
{
	float x, y, z;
	float tu, tv;
};

typedef float matrix4x4[4][4];

//кол-во треугольников
#define CapTriangleCount 12 

//кол-во вертексов
//равно 12 частей (треугольников) + 1 верт.завершающий
//и + 1 вершина в центре = 14
#define CapVertCount (CapTriangleCount + 1 + 1) 

//кол-во индексов
#define CapIndexCount (CapTriangleCount * 3) 

vector3* g_CylinderVertBuff;
vector3* g_CylinderVertBuffTransformed;
unsigned int* g_CylinderIndexBuff;

void Delete_BackBuffer();
void Clear_BackBuffer();
void Present_BackBuffer();
void Create_BackBuffer();

vector3 Vec3_Mat4x4_Mul(vector3& VecIn, matrix4x4 MatIn);

vector3 g_CapBuffTop[CapVertCount];
vector3 g_VertBuffTransformedCapTop[CapVertCount];
unsigned int g_IndexBuffCapTop[CapIndexCount];

vector3 g_CapBuffBottom[CapVertCount];
vector3 g_VertBuffTransformedCapBottom[CapVertCount];

unsigned int g_IndexBuffCapBottom[CapIndexCount];

HWND g_hWnd;

int g_CylinderVertCount;
int g_CylinderTriangleCount;

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

void Scene_Init()
{
	GetClientRect(g_hWnd, &g_Rc);

	Create_BackBuffer();

	float BottomRadius = 5.0f;
	float TopRadius = 5.0f;
	float Height = 12.0f;
	int SliceCount = 20;
	int StackCount = 20;

	float StackHeight = Height / StackCount;
	float RadiusStep = (TopRadius - BottomRadius) / StackCount;
	int RingCount = StackCount + 1;
	g_CylinderTriangleCount = SliceCount * StackCount * 2;

	int RingVertexCount = SliceCount + 1;
	g_CylinderVertCount = RingVertexCount * (StackCount + 1);

	g_CylinderVertBuff = new vector3[g_CylinderVertCount];
	g_CylinderVertBuffTransformed = new vector3[g_CylinderVertCount];

	int Index = 0;
	//рассчитываем вершины для каждого stack ring
	//начиная снизу и двигаясь вверх
	for (int i = 0; i < RingCount; ++i)
	{
		float y = -0.5f * Height + i * StackHeight;
		float r = BottomRadius + i * RadiusStep;

		//вершины кольца
		float dTheta = 2.0f * PI / SliceCount;
		for (int j = 0; j <= SliceCount; ++j)
		{
			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			g_CylinderVertBuff[Index].x = r * c;
			g_CylinderVertBuff[Index].y = y;
			g_CylinderVertBuff[Index].z = r * s;

			g_CylinderVertBuff[Index].tu = (float)j / SliceCount;
			g_CylinderVertBuff[Index].tv = 1.0f - (float)i / StackCount;

			Index++;
		}
	}

	g_CylinderIndexBuff = new unsigned int[g_CylinderTriangleCount * 3];

	Index = 0;

	for (int i = 0; i < StackCount; ++i)
	{
		for (int j = 0; j < SliceCount; ++j)
		{
			g_CylinderIndexBuff[Index + 0] = i * RingVertexCount + j;
			g_CylinderIndexBuff[Index + 1] = (i + 1) * RingVertexCount + j;
			g_CylinderIndexBuff[Index + 2] = (i + 1) * RingVertexCount + j + 1;

			g_CylinderIndexBuff[Index + 3] = i * RingVertexCount + j;
			g_CylinderIndexBuff[Index + 4] = (i + 1) * RingVertexCount + j + 1;
			g_CylinderIndexBuff[Index + 5] = i * RingVertexCount + j + 1;

			Index += 6;
		}
	}

	//шляпа дно цилиндра

	float Dl = PI * 2.0f / CapTriangleCount;

	for (int i = 0; i < CapVertCount - 1; i++)
	{
		float angle = i * Dl;

		g_CapBuffTop[i].x = BottomRadius * sinf(angle);
		g_CapBuffTop[i].y = Height / 2.0f;
		g_CapBuffTop[i].z = BottomRadius * cosf(angle);

		g_CapBuffBottom[i].x = BottomRadius * sinf(angle);
		g_CapBuffBottom[i].y = -Height / 2.0f;
		g_CapBuffBottom[i].z = -BottomRadius * cosf(angle);

		//центр окружности, вершины расположены в центре
		//системы координат (0,0,0)
		//текстурные координаты (0,0) начинаются
		//в левом верхнем углу - делаем преобразование
		g_CapBuffTop[i].tu = 0.5f * (1.0f + sinf(angle));
		g_CapBuffTop[i].tv = 0.5f * (1.0f - cosf(angle));

		g_CapBuffBottom[i].tu = 0.5f * (1.0f + sinf(angle));
		g_CapBuffBottom[i].tv = 0.5f * (1.0f - cosf(angle));
	}

	g_CapBuffTop[CapVertCount - 1].x = 0.0f;
	g_CapBuffTop[CapVertCount - 1].y = Height / 2.0f;
	g_CapBuffTop[CapVertCount - 1].z = 0.0f;

	g_CapBuffBottom[CapVertCount - 1].x = 0.0f;
	g_CapBuffBottom[CapVertCount - 1].y = -Height / 2.0f;
	g_CapBuffBottom[CapVertCount - 1].z = 0.0f;

	for (int i = 0; i < CapTriangleCount; i++)
	{
		g_IndexBuffCapTop[i * 3 + 0] = CapVertCount - 1;
		g_IndexBuffCapTop[i * 3 + 1] = i;
		g_IndexBuffCapTop[i * 3 + 2] = i + 1;

		g_IndexBuffCapBottom[i * 3 + 0] = CapVertCount - 1;
		g_IndexBuffCapBottom[i * 3 + 1] = i;
		g_IndexBuffCapBottom[i * 3 + 2] = i + 1;
	}

}

void Scene_Update()
{
	static float AngleX = 5.246261f;
	static float AngleY = 5.246261f;

	matrix4x4 MatRotateY = {
		cosf(AngleY),	0.0,	-sinf(AngleY),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(AngleY),	0.0,	cosf(AngleY),	0.0,
		0.0,			0.0,	0.0,			1.0f };

	matrix4x4 MatRotateX = {
		1.0,			0.0,			0.0,			0.0,
		0.0,			cosf(AngleX),	sinf(AngleX),	0.0,
		0.0,			-sinf(AngleX),	cosf(AngleX),	0.0,
		0.0,			0.0,			0.0,			1.0f };

	/*
		AngleX = AngleX + PI / 100.0f;
		if(AngleX > PI2 )
			AngleX = 0.0f;

		AngleY = AngleY + PI / 100.0f;
		if(AngleY > PI2 )
			AngleY = 0.0f;
	*/

	//MATRIX VIEW CALCULATION
	vector3 VecCamRight = { 1.0f, 0.0f, 0.0 };
	vector3 VecCamUp = { 0.0f, 1.0f, 0.0f };
	vector3 VecCamPos = { 0.0f, 9.0f, -15.0f };
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
		Px,					Py,			Pz,					1.0f };

	for (int i = 0; i < g_CylinderVertCount; i++)
	{
		vector3 VecTemp = Vec3_Mat4x4_Mul(g_CylinderVertBuff[i], MatRotateX);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateY);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatView);

		VecTemp.x = VecTemp.x / VecTemp.z;
		VecTemp.y = VecTemp.y / VecTemp.z;

		VecTemp.x = VecTemp.x / ((float)g_ViewWidth / g_ViewHeight);

		VecTemp.x = VecTemp.x * g_ViewWidth / 2.0f + g_ViewWidth / 2.0f;
		VecTemp.y = -VecTemp.y * g_ViewHeight / 2.0f + g_ViewHeight / 2.0f;

		g_CylinderVertBuffTransformed[i] = VecTemp;
	}

	//шляпа цилиндра круг из треугольников
	for (int i = 0; i < CapVertCount; i++)
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_CapBuffTop[i], MatRotateX);
		Vec = Vec3_Mat4x4_Mul(Vec, MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatView);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ((float)g_ViewWidth / g_ViewHeight);

		Vec.x = Vec.x * g_ViewWidth / 2.0f + g_ViewWidth / 2.0f;
		Vec.y = -Vec.y * g_ViewHeight / 2.0f + g_ViewHeight / 2.0f;

		g_VertBuffTransformedCapTop[i] = Vec;
	}

	for (int i = 0; i < CapVertCount; i++)
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_CapBuffBottom[i], MatRotateX);
		Vec = Vec3_Mat4x4_Mul(Vec, MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatView);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ((float)g_ViewWidth / g_ViewHeight);

		Vec.x = Vec.x * g_ViewWidth / 2.0f + g_ViewWidth / 2.0f;
		Vec.y = -Vec.y * g_ViewHeight / 2.0f + g_ViewHeight / 2.0f;

		g_VertBuffTransformedCapBottom[i] = Vec;
	}
}

void Scene_Draw()
{
	Clear_BackBuffer();

	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(g_hBackBuffer, hPen);

	for (int i = 0; i < g_CylinderTriangleCount; i++)
	{
		vector3 Vec1 = g_CylinderVertBuffTransformed[g_CylinderIndexBuff[i * 3 + 0]];
		vector3 Vec2 = g_CylinderVertBuffTransformed[g_CylinderIndexBuff[i * 3 + 1]];
		vector3 Vec3 = g_CylinderVertBuffTransformed[g_CylinderIndexBuff[i * 3 + 2]];

		//используем псевдоскалярное (косое) произведение векторов
		//для отбрасывания задних поверхностей
		//координаты в экранных координатах
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if (s <= 0)
			continue;

		MoveToEx(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(g_hBackBuffer, (int)Vec2.x, (int)Vec2.y);
		LineTo(g_hBackBuffer, (int)Vec3.x, (int)Vec3.y);
		LineTo(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y);
	}

	for (int i = 0; i < CapTriangleCount; i++)
	{
		vector3 Vec1 = g_VertBuffTransformedCapTop[g_IndexBuffCapTop[i * 3 + 0]];
		vector3 Vec2 = g_VertBuffTransformedCapTop[g_IndexBuffCapTop[i * 3 + 1]];
		vector3 Vec3 = g_VertBuffTransformedCapTop[g_IndexBuffCapTop[i * 3 + 2]];

		//используем псевдоскалярное (косое) произведение векторов
		//для отбрасывания задних поверхностей
		//координаты в экранных координатах
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if (s <= 0)
			continue;

		MoveToEx(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(g_hBackBuffer, (int)Vec2.x, (int)Vec2.y);
		LineTo(g_hBackBuffer, (int)Vec3.x, (int)Vec3.y);
		LineTo(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y);
	}

	for (int i = 0; i < CapTriangleCount; i++)
	{
		vector3 Vec1 = g_VertBuffTransformedCapBottom[g_IndexBuffCapTop[i * 3 + 0]];
		vector3 Vec2 = g_VertBuffTransformedCapBottom[g_IndexBuffCapTop[i * 3 + 1]];
		vector3 Vec3 = g_VertBuffTransformedCapBottom[g_IndexBuffCapTop[i * 3 + 2]];

		//используем псевдоскалярное (косое) произведение векторов
		//для отбрасывания задних поверхностей
		//координаты в экранных координатах
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if (s <= 0)
			continue;

		MoveToEx(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(g_hBackBuffer, (int)Vec2.x, (int)Vec2.y);
		LineTo(g_hBackBuffer, (int)Vec3.x, (int)Vec3.y);
		LineTo(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y);
	}

	SelectObject(g_hBackBuffer, hOldPen);
	DeleteObject(hPen);

	Present_BackBuffer();

	Sleep(50);
}

void Scene_Destory()
{
	Delete_BackBuffer();

	delete[] g_CylinderVertBuff;
	g_CylinderVertBuff = NULL;

	delete[] g_CylinderVertBuffTransformed;
	g_CylinderVertBuffTransformed = NULL;

	delete[] g_CylinderIndexBuff;
	g_CylinderIndexBuff = NULL;
};


void Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(g_hWnd, &g_Rc);

	g_ViewWidth = g_Rc.right;
	g_ViewHeight = g_Rc.bottom;

	g_hDC = GetDC(g_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	//g_hBrush = CreateSolidBrush(RGB(0,0,0));
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

	Scene_Init();

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

		Scene_Update();
		Scene_Draw();
	}

	Scene_Destory();
	
	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}