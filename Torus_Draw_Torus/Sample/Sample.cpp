#include <windows.h>
#include <math.h>
#include <stdio.h>

HWND g_hWnd;

int g_VertCount;
int g_TriangleCount;

struct vertex
{
	float x,y,z;

	vertex operator - (vertex VecIn)
	{
		vertex VecOut;

		VecOut.x = x - VecIn.x;
		VecOut.y = y - VecIn.y;
		VecOut.z = z - VecIn.z;

		return VecOut;
	}
};

float Vec3_Dot(vertex VecIn1, vertex VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

vertex Vec3_Normalize(vertex VecIn)
{
	float len = sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);

	vertex VecOut;

	VecOut.x = VecIn.x / len;
	VecOut.y = VecIn.y / len;
	VecOut.z = VecIn.z / len;

	return VecOut;
}

vertex Vec3_Cross(vertex VecIn1, vertex VecIn2)
{
	vertex VecOut;

	VecOut.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	VecOut.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	VecOut.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return VecOut;
}

vertex *g_VertBuff;
vertex *g_VertBuffTransformed;
unsigned int *g_IndexBuff;

typedef float matrix4x4[4][4];

void Delete_Torus()
{
	delete [] g_VertBuff;
	delete [] g_VertBuffTransformed;
	delete [] g_IndexBuff;
}

HDC g_hBackBuffer;
HBITMAP g_hBitmap;
HBITMAP g_hOldBitmap;

void Create_BackBuffer()
{
	HDC hDC = GetDC(g_hWnd);
	g_hBackBuffer = CreateCompatibleDC(hDC);
	g_hBitmap = CreateCompatibleBitmap(hDC, 800, 600);
	g_hOldBitmap = (HBITMAP) SelectObject(g_hBackBuffer, g_hBitmap);
	ReleaseDC(g_hWnd, hDC);
}

void Delete_BackBuffer()
{
	SelectObject(g_hBackBuffer, g_hOldBitmap);
	DeleteObject(g_hBitmap);
	DeleteDC(g_hBackBuffer);
}

void Clear_BackBuffer()
{
	HBRUSH hBrush = CreateSolidBrush( RGB(255,255,255) );

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	FillRect(g_hBackBuffer, &Rc, hBrush);
}

void Present_BackBuffer()
{
	HDC hDC = GetDC(g_hWnd);
	BitBlt(hDC, 0, 0, 800, 600, g_hBackBuffer, 0, 0, SRCCOPY);
	ReleaseDC(g_hWnd, hDC);
}

vertex Vec3_Mat4x4_Mul ( vertex VecIn, matrix4x4 MatIn)
{
	vertex VecOut;

	VecOut.x =		VecIn.x * MatIn[0][0] +
					VecIn.y * MatIn[1][0] +
					VecIn.z * MatIn[2][0] +
							  MatIn[3][0];

	VecOut.y =		VecIn.x * MatIn[0][1] +
					VecIn.y * MatIn[1][1] +
					VecIn.z * MatIn[2][1] +
							  MatIn[3][1];

	VecOut.z =		VecIn.x * MatIn[0][2] +
					VecIn.y * MatIn[1][2] +
					VecIn.z * MatIn[2][2] +
							  MatIn[3][2];

	return VecOut;
}

void Init_Torus()
{
	
	int Rings = 24;
	int Sides = 12;

	g_TriangleCount = 2 * Sides * Rings;
    //одно дополнительное кольцо для дублирования первого кольца
	g_VertCount  = Sides * (Rings+1);

	g_VertBuff = new vertex[g_VertCount];
	g_VertBuffTransformed = new vertex[g_VertCount];

	float OuterRadius = 8; //внешний радиус (общий)
	float InnerRadius = 4; //радиус трубки

	float PI2 = 3.1415926f * 2.0f;

	float RingFactor = PI2 / Rings;
	float SideFactor = PI2 / Sides;

	int Index = 0;

    for( int Ring = 0; Ring <= Rings; Ring++ )
    {
        float u = Ring * RingFactor;
        float cu = cosf(u);
        float su = sinf(u);

        for( int Side = 0; Side < Sides; Side++ )
        {
            float VecIn = Side * SideFactor;
            float cv = cosf(VecIn);
            float sv = sinf(VecIn);
            float r = (OuterRadius + InnerRadius * cv);

            g_VertBuff[Index].x = r * cu;
            g_VertBuff[Index].y = r * su;
            g_VertBuff[Index].z = InnerRadius * sv;

			Index++;
        }
    }

	g_IndexBuff = new unsigned int[g_TriangleCount * 3];

    Index = 0;

    for( int Ring = 0; Ring < Rings; Ring++ )
    {
        int ringStart = Ring * Sides;
        int nextRingStart = (Ring + 1) * Sides;
    
        for( int Side = 0; Side < Sides; Side++ )
        {
            int nextSide = (Side + 1) % Sides;
            // квадрат - два треугольника
            g_IndexBuff[Index + 0] = (ringStart + Side);
            g_IndexBuff[Index + 1] = (nextRingStart + Side);
            g_IndexBuff[Index + 2] = (nextRingStart + nextSide);
            g_IndexBuff[Index + 3] = ringStart + Side;
            g_IndexBuff[Index + 4] = nextRingStart + nextSide;
            g_IndexBuff[Index + 5] = (ringStart + nextSide);
            
            Index += 6;
        }
    }
}

void Draw_Torus()
{
	static float Angle = 2.670355f;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		-sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0f };

	matrix4x4 MatRotateX = {
		1.0,			0.0,			0.0,			0.0,
		0.0,			cosf(Angle),	sinf(Angle),	0.0,
		0.0,			-sinf(Angle),	cosf(Angle),	0.0,
		0.0,			0.0,			0.0,			1.0f };

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	for ( int i = 0; i < g_VertCount; i++ )
	{
		vertex VecIn = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRotateX);
		VecIn = Vec3_Mat4x4_Mul(VecIn, MatRotateY);

		VecIn.x = VecIn.x / ((float)Rc.right / Rc.bottom);
		VecIn.z = VecIn.z + 25.0f;
		VecIn.x = VecIn.x / VecIn.z;
		VecIn.y = VecIn.y / VecIn.z;

		VecIn.x = VecIn.x * Rc.right / 2.0f + Rc.right / 2.0f;
		VecIn.y =-VecIn.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = VecIn;
	}

	Clear_BackBuffer();
	
	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0) );
	HPEN hOldPen = (HPEN) SelectObject(g_hBackBuffer, hPen);

	for ( int i = 0; i < g_TriangleCount; i++ )
	{
		vertex Vec1 = g_VertBuffTransformed[g_IndexBuff[i * 3 + 0]];
		vertex Vec2 = g_VertBuffTransformed[g_IndexBuff[i * 3 + 1]];
		vertex Vec3 = g_VertBuffTransformed[g_IndexBuff[i * 3 + 2]];

		vertex Edge1, Edge2, Cross;

		Edge1 = Vec2 - Vec1;
		Edge2 = Vec3 - Vec1;

		Edge1 = Vec3_Normalize(Edge1);
		Edge2 = Vec3_Normalize(Edge2);

		Cross = Vec3_Cross(Edge2, Edge1);
		Cross = Vec3_Normalize(Cross);

		vertex Look = {0.0f, 0.0f, -1.0f};

		float Dot = Vec3_Dot(Cross, Look);

		if(Dot <= 0.0f)
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

LRESULT CALLBACK WndProc ( HWND g_hWnd,
						  UINT uMsg,
						  WPARAM wParam,
						  LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc( g_hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain ( HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow )
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc = WndProc;
	wcl.cbClsExtra = 0L;
	wcl.cbWndExtra = 0L;
	wcl.hInstance = hInstance;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = "Sample";

	if ( !RegisterClass(&wcl) )
		return 0;

	g_hWnd = CreateWindow("Sample", "Sample 3D Application - Ed Kurlyak",
					WS_OVERLAPPEDWINDOW,
					0, 0,
					800, 600,
					NULL,
					NULL,
					hInstance,
					NULL);

	if ( !g_hWnd )
		return 0;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	Create_BackBuffer();
	Init_Torus();

	MSG msg;

	while ( true )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if ( GetKeyState(VK_ESCAPE) & 0xFF00 )
			break;

		Draw_Torus();
	}

	Delete_Torus();
	Delete_BackBuffer();

	DestroyWindow(g_hWnd);
	UnregisterClass("Sample", hInstance);

	return 0;
}