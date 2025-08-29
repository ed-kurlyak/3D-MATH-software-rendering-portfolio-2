//======================================================================================
//	Ed Kurlyak 2020 Sphere Mesh Software
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#define EPSILON_E3 (float) 0.005f
// floating point comparison
#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? 1 : 0)

struct matrix4x4
{
	float m[4][4];
};

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{
	vector3() {};
	vector3(float xi, float yi, float zi) : x(xi), y(yi), z(zi) {};

	union
	{
		float m[3];
		struct {
			float x,y,z;
		};
	};


	vector3 operator - (const vector3 VecIn)
	{
		vector3 VecOut;
		
		VecOut.x = x - VecIn.x;
		VecOut.y = y - VecIn.y;
		VecOut.z = z - VecIn.z;
		
		return VecOut;
	}
	
	vector3 &operator = (const vector3 & VecIn)
	{
		x = VecIn.x;
		y = VecIn.y;
		z = VecIn.z;

		return *this;
	}
};

class CMeshManager
{
public:
	CMeshManager() {};
	~CMeshManager() {};

	void InitSphere(HWND hWnd);
	void CalculateSphere();
	void DrawSphere (HWND) ;

private:
	vector3 Mat4x4_Vect3_Mul(vector3 VecIn, matrix4x4 MatIn);
	float Vec3Dot(vector3 VecIn1, vector3 VecIn2);
	vector3 Vec3Cross(vector3 VecIn1, vector3 VecIn2);
	vector3 Vec3Normalize(vector3 VecIn);

	HWND m_hWnd;

	UINT m_VertCount;
	UINT m_IndexCount;

	ULONG m_ViewX;
	ULONG m_ViewY;
	ULONG m_ViewWidth;
	ULONG m_ViewHeight;
	
	vector3 *m_VertBuff;
	vector3* m_VertBuffTransformed;
	DWORD *m_IndexBuff;

};

#endif