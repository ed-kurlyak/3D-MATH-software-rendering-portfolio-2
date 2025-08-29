//======================================================================================
//	Ed Kurlyak 2020 Sphere Mesh Software
//======================================================================================

#include "MeshManager.h"

vector3 CMeshManager::Mat4x4_Vect3_Mul(vector3 VecIn, matrix4x4 MatIn )
{
	vector3 Temp;

	for ( int j = 0; j < 3; j++)
	{
		float Sum = 0.0f;
		int i;
		for ( i = 0; i < 3; i++)
		{
			Sum += VecIn.m[i] * MatIn.m[i][j];
		}
		
		Sum += MatIn.m[i][j];
		Temp.m[j] = Sum;
	}

	return Temp;
}

float CMeshManager::Vec3Dot(vector3 VecIn1, vector3 VecIn2)
{
	return (VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z);
}

vector3 CMeshManager::Vec3Cross(vector3 VecIn1, vector3 VecIn2)
{
	vector3 Temp;

	Temp.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	Temp.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	Temp.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return Temp;
}

vector3 CMeshManager::Vec3Normalize(vector3 VecIn)
{
	vector3 t;

	float vlen = sqrtf( (VecIn.x * VecIn.x) + (VecIn.y * VecIn.y) + (VecIn.z * VecIn.z));

	t.x = VecIn.x/vlen;
	t.y = VecIn.y/vlen;
	t.z = VecIn.z/vlen;

	return t;
}

void CMeshManager::InitSphere(HWND hWnd)
{

	m_hWnd = hWnd;

	RECT Rc;
    ::GetClientRect( m_hWnd, &Rc );

    m_ViewX      = Rc.left;
    m_ViewY      = Rc.top;
    m_ViewWidth  = Rc.right - Rc.left;
    m_ViewHeight = Rc.bottom - Rc.top;
	
	int Stacks = 10;
	int Slices = 20;
	float TWOPI  = 6.28318530717958f;
	float PI     = 3.14159265358979f;
	float ThetaFac = TWOPI / (float)Slices;
	float PhiFac = PI / (float)Stacks;
	float r = 15.0;

	m_VertCount =  Slices * Stacks * 4;
	m_IndexCount =  Slices * Stacks * 2;

	m_VertBuff = NULL;
	m_VertBuff = new vector3[m_VertCount];
	
	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[m_IndexCount  * 3];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[m_VertCount];

	int Index = 0;
	
	for(int l = 0; l < Slices; l++)
	{
		for(int b = 0; b < Stacks; b++)
		{
			float sb = PhiFac * b;
			float sl = ThetaFac * l;

			m_VertBuff[Index].z = r * sinf(sb) * cosf(sl);
			m_VertBuff[Index].x = r * sinf(sb) * sinf(sl);
			m_VertBuff[Index].y = r * cosf(sb);

			sb = PhiFac * (b + 1);
			sl = ThetaFac * l;

			m_VertBuff[Index + 1].z = r * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 1].x = r * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 1].y = r * cosf(sb);
			
			sb = PhiFac * (b + 1);
			sl = ThetaFac * (l + 1);

			m_VertBuff[Index+2].z = r * sinf(sb) * cosf(sl);
			m_VertBuff[Index+2].x = r * sinf(sb) * sinf(sl);
			m_VertBuff[Index+2].y = r * cosf(sb);

			sb = PhiFac * b;
			sl = ThetaFac * (l + 1);

			m_VertBuff[Index+3].z = r * sinf(sb) * cosf(sl);
			m_VertBuff[Index+3].x = r * sinf(sb) * sinf(sl);
			m_VertBuff[Index+3].y = r * cosf(sb);

			Index+=4;
		}
	}

	int BaseIndex = 0;
	UINT BaseVert = 0;
		
		while (BaseVert < m_VertCount)
		{
			m_IndexBuff[BaseIndex + 0] = BaseVert;
			m_IndexBuff[BaseIndex + 1] = BaseVert+1;
			m_IndexBuff[BaseIndex + 2] = BaseVert+2;

			m_IndexBuff[BaseIndex + 3] = BaseVert;
			m_IndexBuff[BaseIndex + 4] = BaseVert+2;
			m_IndexBuff[BaseIndex + 5] = BaseVert+3;

			BaseIndex += 6;
			BaseVert += 4;
		}
}

void CMeshManager::CalculateSphere()
{
	static float Angle = 5.515244f;

	matrix4x4 MatWorldRotX={
		1,		0,				0,				0,
        0,		cosf(Angle),	sinf(Angle),	0,
        0,		-sinf(Angle),	cosf(Angle),	0,
        0,		0,				0,				1};

	matrix4x4 MatWorldRotY={
		cosf(Angle),	0,		-sinf(Angle),	0,
		0,				1,		0,				0,
		sinf(Angle),	0,		cosf(Angle),	0,
		0,				0,		0,				1};

	matrix4x4 MatWorldRotZ={
		cosf(Angle),	sinf(Angle),	0,	0,
		-sinf(Angle),	cosf(Angle),	0,	0,
        0,				0,				1,	0,
        0,				0,				0,	1};

	/*
	Angle+=(float)(3.1415926/45.0);
    if(Angle>3.1415926*2)
		Angle = 0;
	*/

	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 31, 1};

	matrix4x4 MatView={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	float Fov =(float) 3.14/2; // FOV 90 degree
    float Aspect = (float) m_ViewWidth/m_ViewHeight;
	float ZFar = 100.0f;
	float ZNear = (float)0.1f;

	float    h, w, Q;
    w = (float)(1/tanf(Fov*0.5f))/Aspect;  
    h = (float)1/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - ZNear);

	/*
    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*ZNear, 0 };
	*/

	matrix4x4 MatProj = {
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 1 };

	for (UINT i = 0; i < m_VertCount; i++)
	{
		vector3 Vec;

		Vec = Mat4x4_Vect3_Mul(m_VertBuff[i], MatWorldRotY);
		Vec = Mat4x4_Vect3_Mul(Vec, MatWorldRotX);
		Vec = Mat4x4_Vect3_Mul(Vec, MatWorldRotZ);
		Vec = Mat4x4_Vect3_Mul(Vec, MatWorld);
		Vec = Mat4x4_Vect3_Mul(Vec, MatView);
		Vec = Mat4x4_Vect3_Mul(Vec, MatProj);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		m_VertBuffTransformed[i] = Vec;
	}
}

void CMeshManager::DrawSphere (HWND hwnd)
{

	HDC hDC = ::GetDC( m_hWnd );

	LOGBRUSH logBrush;
	HBRUSH   hBrush = NULL, hOldBrush = NULL;
    
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = RGB(255, 255, 255) ;
    
	hBrush = ::CreateBrushIndirect( &logBrush );
	if (!hBrush) return;

	RECT Rc;

	GetClientRect(m_hWnd, &Rc);

	FillRect(hDC, &Rc, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject (hDC, hPen) ;

    for (UINT i = 0; i < m_IndexCount; i++)
    {
		DWORD n1 = m_IndexBuff[i * 3 + 0];
		DWORD n2 = m_IndexBuff[i * 3 + 1];
		DWORD n3 = m_IndexBuff[i * 3 + 2];

		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];
		
		if( FCMP(Vec1.x, Vec3.x) && FCMP(Vec1.y, Vec3.y) )
			continue;
		
		if( FCMP(Vec2.x, Vec3.x) && FCMP(Vec2.y, Vec3.y) )
			continue;

		vector3 Edge1, Edge2, Cross, Look;

		Edge1 = Vec2 - Vec1;
		Edge2 = Vec3 - Vec1;

		Edge1 = Vec3Normalize(Edge1);
		Edge2 = Vec3Normalize(Edge2);

		Cross = Vec3Cross(Edge1, Edge2);

		Cross = Vec3Normalize(Cross);

		Look.x = 0.0f;
		Look.y = 0.0f;
		Look.z = -1.0f;
		float AngleCos = Vec3Dot(Cross, Look);

		if(AngleCos <= 0.0f) //backface culling
			continue;
		
		float Alpha = (0.5f * m_ViewWidth - 0.5f);
		float Beta  = (0.5f * m_ViewHeight - 0.5f);
	
		matrix4x4 mScreen = {
			Alpha,  0,	    0,    0, 
			0,      -Beta,  0,    0, 
			0,		0,		1,    0,
			Alpha,  Beta,	0,    1};

		Vec1 = Mat4x4_Vect3_Mul(Vec1, mScreen);
		Vec2 = Mat4x4_Vect3_Mul(Vec2, mScreen);
		Vec3 = Mat4x4_Vect3_Mul(Vec3, mScreen);

		MoveToEx(hDC,(int)Vec1.x, (int)Vec1.y,NULL);
        LineTo(hDC,(int)Vec2.x, (int)Vec2.y);
		LineTo(hDC,(int)Vec3.x, (int)Vec3.y);
		LineTo(hDC,(int)Vec1.x, (int)Vec1.y);
     }  

	::SelectObject(hDC,hOldPen);
	::DeleteObject (hPen) ;

	::SelectObject( hDC, hOldBrush );
	::DeleteObject( hBrush );

	ReleaseDC(m_hWnd, hDC);
}

