//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
};

CMeshManager::~CMeshManager()
{
	Delete_BackBuffer();

	delete [] m_VertBuff;
	m_VertBuff = NULL;

	delete [] m_VertBuffTransformed;
	m_VertBuffTransformed = NULL;

	delete [] m_IndexBuff;
	m_IndexBuff = NULL;

	delete [] m_ColorBuff;
	m_ColorBuff = NULL;
}

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
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

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	Create_BackBuffer();

	m_VertCount = 5;
	m_TriangleCount = 6;
	
	vector3 VertBuffTemp[5] = {
		-4.0f,	-4.0f,	-4.0f,	//A
		 4.0f,	-4.0f,	-4.0f,	//B
		-4.0f,	-4.0f,	 4.0f,	//C 
		 4.0f,	-4.0f,	 4.0f,	//D 
		 0.0f,	 4.0f,	 0.0f };	//E Top

	DWORD IndexBuffTemp[6 * 3] = {
		C, A, B,	//bottom
		C, B, D,	//bottom
		A, E, B,	//front
		B, E, D,	//rigth
		D, E, C,	//back
		C, E, A	};	//left

	m_VertBuff = NULL;
	m_VertBuff = new vector3[5];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[5];

	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[6 * 3];

	m_ColorBuff = NULL;
	m_ColorBuff = new color_rgb[6];

	memcpy(m_VertBuff, VertBuffTemp, 5 * sizeof(vector3));
	memcpy(m_IndexBuff, IndexBuffTemp, 6 * 3 * sizeof(DWORD));
}

void CMeshManager::Update_MeshManager()
{
	static float Angle = 2.576107f;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,	0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,	1.0 };

	matrix4x4 MatRotateX = {
		1.0,			0.0,			0.0,			0.0,
		0.0,			cosf(Angle),	sinf(Angle),	0.0,
		0.0,			-sinf(Angle),	cosf(Angle),	0.0,
		0.0,			0.0,			0.0,			1.0 };

	/*
	Angle = Angle + PI / 50.0f;
	if (Angle > PI2)
		Angle = 0.0f;
	*/

	for ( UINT i = 0; i < m_VertCount; i++ )
	{
		vector3 VecTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateY);
	
		VecTemp.z = VecTemp.z + 12.0f;
		VecTemp.x = VecTemp.x / VecTemp.z;
		VecTemp.y = VecTemp.y / VecTemp.z;

		VecTemp.x = VecTemp.x / ((float)m_ViewWidth / m_ViewHeight);

		VecTemp.x = VecTemp.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		VecTemp.y =-VecTemp.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		m_VertBuffTransformed[i] = VecTemp;
	}
}

void CMeshManager::Draw_MeshManager()
{
	Clear_BackBuffer();

	for ( UINT i = 0; i< m_TriangleCount; i++ )
	{
		vector3 Vec1 = m_VertBuffTransformed[ m_IndexBuff [ i * 3 + 0 ] ];
		vector3 Vec2 = m_VertBuffTransformed[ m_IndexBuff [ i * 3 + 1 ] ];
		vector3 Vec3 = m_VertBuffTransformed[ m_IndexBuff [ i * 3 + 2 ] ];

		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if(s <= 0)
			continue;

		HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0,0,0));
		HPEN hOldPen = (HPEN) SelectObject(m_hBackBuffer, hPen);

		MoveToEx(m_hBackBuffer, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(m_hBackBuffer, (int)Vec2.x, (int)Vec2.y);
		LineTo(m_hBackBuffer, (int)Vec3.x, (int)Vec3.y);
		LineTo(m_hBackBuffer, (int)Vec1.x, (int)Vec1.y);

		SelectObject(m_hBackBuffer, hOldPen);
		DeleteObject(hPen);
	}
	
	Present_BackBuffer();

	Sleep(25);
}

void CMeshManager::Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(m_hWnd, &m_Rc);

	m_ViewWidth = m_Rc.right;
	m_ViewHeight = m_Rc.bottom;

	m_hDC = GetDC(m_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	m_hBrush = CreateSolidBrush(RGB(255,255,255));

	//наш задний буфер
	m_hBackBuffer = CreateCompatibleDC(m_hDC);
	m_hBitmap = CreateCompatibleBitmap(m_hDC, m_ViewWidth, m_ViewHeight);
	m_hOldBitmap = (HBITMAP) SelectObject(m_hBackBuffer, m_hBitmap);
}

void CMeshManager::Clear_BackBuffer()
{
	//очистка окна приложения
	FillRect(m_hBackBuffer, &m_Rc, m_hBrush);
}

void CMeshManager::Present_BackBuffer()
{
	//выводим задний буфер на экран приложения
	BitBlt(m_hDC, 0, 0, m_ViewWidth, m_ViewHeight, m_hBackBuffer, 0, 0, SRCCOPY);
}

void CMeshManager::Delete_BackBuffer()
{
	//уничтожение заднего буфера
	SelectObject(m_hBackBuffer, m_hOldBitmap);
	DeleteObject(m_hBitmap);

	DeleteObject(m_hBrush);

	ReleaseDC(m_hWnd, m_hDC);
	DeleteDC(m_hBackBuffer);
}

