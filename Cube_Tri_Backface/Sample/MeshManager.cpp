//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{ 
	delete [] m_VertBuff;
	m_VertBuff = NULL;

	delete [] m_VertBuffTransformed;
	m_VertBuffTransformed = NULL;

	delete [] m_IndexBuff;
	m_IndexBuff = NULL;
}

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
{
	    vector3 VecOut;

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

vector3 CMeshManager::Vec3_Normalize(vector3 &VecIn)
{
	vector3 VecOut;

	float Len = sqrtf( (VecIn.x * VecIn.x) + (VecIn.y * VecIn.y) + (VecIn.z * VecIn.z));

	VecOut.x = VecIn.x / Len;
	VecOut.y = VecIn.y / Len;
	VecOut.z = VecIn.z / Len;

	return VecOut;
}

float CMeshManager::Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

vector3 CMeshManager::Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	VecOut.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	VecOut.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return VecOut;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	::GetClientRect( m_hWnd, &m_Rc );

	m_VertCount = 8;
	m_TriangleCount = 12;

	m_VertBuff = NULL;
	m_VertBuff = new vector3[m_VertCount];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[m_VertCount];
	
	m_IndexBuff = NULL;
	m_IndexBuff = new unsigned int[m_TriangleCount * 3];

	/*
			front face		backface

			C - D			G - H
			|	|			|	|
			A - B			E - F
	*/


	vector3 VertBuffTemp[8] = {
		-4.0f,	-4.0f,	-4.0f,
		 4.0f,	-4.0f,	-4.0f,
		-4.0f,	 4.0f,	-4.0f,
		 4.0f,	 4.0f,	-4.0f,

		-4.0f,	-4.0f,	 4.0f,
		 4.0f,	-4.0f,	 4.0f,
		-4.0f,	 4.0f,	 4.0f,
		 4.0f,	 4.0f,	 4.0f };

	DWORD IndexBuffTemp[36] = {
	//front face	
	A, C, D,
	A, D, B,
	
	//left face
	E, G, C,
	E, C, A,

	//back face
	G, E, F,
	G, F, H,

	//right face
	B, D, H,
	B, H, F,

	//top face
	C, G, H,
	C, H, D,

	//bottom face
	E, A, B,
	E, B, F};

	memcpy(m_VertBuff, VertBuffTemp, m_VertCount * sizeof(vector3));
	memcpy(m_IndexBuff, IndexBuffTemp, m_TriangleCount * 3 * sizeof(unsigned int));
		
}

void CMeshManager::Update_MeshManager()
{
	static float Angle = 2.676107f;

	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, cosf(Angle), sinf(Angle), 0,
        0,-sinf(Angle),  cosf(Angle), 0,
        0, 0, 0, 1};

	matrix4x4 MatRotateY={
		cosf(Angle), 0, -sinf(Angle), 0,
		0, 1, 0, 0,
		sinf(Angle), 0, cosf(Angle), 0,
		0, 0, 0, 1};

	matrix4x4 MatRotateZ={
		cosf(Angle), sinf(Angle), 0, 0,
		-sinf(Angle), cosf(Angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	/*
	Angle += PI / 100.0f;
    if(Angle > PI2)
		Angle = 0;
	*/

	matrix4x4 MatWorld={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	vector3 VecRight = { 1.0, 0.0, 0.0 };
	vector3 VecUp = { 0.0, 1.0, 0.0 };
	vector3 VecCamPos = { 0.0, 8.0, -12.0 };
	vector3 VecLook = { VecCamPos.x * -1.0f, VecCamPos.y * -1.0f, VecCamPos.z * -1.0f };

	VecLook = Vec3_Normalize(VecLook);
	VecUp = Vec3_Cross(VecLook,VecRight);
	VecUp = Vec3_Normalize(VecUp);
	VecRight = Vec3_Cross(VecUp,VecLook);
	VecRight = Vec3_Normalize(VecRight);

	float Px = -Vec3_Dot(VecCamPos, VecRight);
	float Py = -Vec3_Dot(VecCamPos, VecUp);
	float Pz = -Vec3_Dot(VecCamPos, VecLook);

	matrix4x4 MatView={
        VecRight.x, VecUp.x, VecLook.x, 0,
		VecRight.y, VecUp.y, VecLook.y, 0,
		VecRight.z, VecUp.z, VecLook.z, 0,
	    Px, Py, Pz, 1};

	float Fov = PI / 2.0f; // FOV 90 degree
	float Aspect = (float) m_Rc.right / m_Rc.bottom;
	float ZFar = 100.0f;
	float ZNear = 0.1f;

	float h, w, Q;
    w = (1.0f/tanf(Fov*0.5f))/Aspect;  
    h = 1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - ZNear);

	/*
    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*ZNear, 0};
	*/

    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};


	float Alpha = 0.5f * m_Rc.right;
	float Beta  = 0.5f * m_Rc.bottom;
	
	matrix4x4 MatScreen = {
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1};

	for (UINT i = 0; i < m_VertCount; i++)
	{
		vector3 Vec;

		Vec = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatWorld);
		Vec = Vec3_Mat4x4_Mul(Vec, MatView);
		Vec = Vec3_Mat4x4_Mul(Vec, MatProj);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec = Vec3_Mat4x4_Mul(Vec, MatScreen);

		m_VertBuffTransformed[i] = Vec;
	}
}

void CMeshManager::Draw_MeshManager()
{
	HDC hDC = ::GetDC( m_hWnd );

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));

	FillRect(hDC, &m_Rc, hBrush);
	
	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject (hDC, hPen) ;

    for (UINT i = 0; i < m_TriangleCount; i++)
    {
		vector3 Vec1 = m_VertBuffTransformed[ m_IndexBuff[i * 3 + 0] ];
		vector3 Vec2 = m_VertBuffTransformed[ m_IndexBuff[i * 3 + 1] ];
		vector3 Vec3 = m_VertBuffTransformed[ m_IndexBuff[i * 3 + 2] ];

		vector3 VecEdge1 = Vec2 - Vec1;
		vector3 VecEdge2 = Vec3 - Vec1;

		VecEdge1 = Vec3_Normalize(VecEdge1);
		VecEdge2 = Vec3_Normalize(VecEdge2);

		//находим нормаль к треугольнику
		//векторы в экранных координатах +Y вниз
		vector3 VecCross = Vec3_Cross(VecEdge2, VecEdge1);

		VecCross = Vec3_Normalize(VecCross);

		vector3 VecLook = { 0.0f, 0.0f, -1.0f };

		float Dot = Vec3_Dot(VecCross, VecLook);
		
		if(Dot <= 0.0) //backface culling
			continue;
	
		MoveToEx(hDC,(int)Vec1.x, (int)Vec1.y,NULL);
        LineTo(hDC,(int)Vec2.x, (int)Vec2.y);
		LineTo(hDC,(int)Vec3.x, (int)Vec3.y);
		LineTo(hDC,(int)Vec1.x, (int)Vec1.y);
     }  

	::SelectObject(hDC,hOldPen);
	::DeleteObject (hPen) ;

	::DeleteObject( hBrush );

	ReleaseDC(m_hWnd, hDC);

	Sleep(50);
}

