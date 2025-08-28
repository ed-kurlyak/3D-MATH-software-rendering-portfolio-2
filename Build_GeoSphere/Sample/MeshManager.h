//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

struct vector3
{
	float x,y,z;

	float tu,tv;

	vector3 operator + (const vector3 & VecIn)
	{
		vector3 VecOut;
		
		VecOut.x = x + VecIn.x;
		VecOut.y = y + VecIn.y;
		VecOut.z = z + VecIn.z;

		VecOut.tu = tu + VecIn.tu;
		VecOut.tv = tv + VecIn.tv;

		return VecOut;
	}

	vector3 operator * (const float & ValIn)
	{
		vector3 VecOut;

		VecOut.x = x * ValIn;
		VecOut.y = y * ValIn;
		VecOut.z = z * ValIn;

		VecOut.tu = tu * ValIn;
		VecOut.tv = tv * ValIn;

		return VecOut;
	}


};

typedef float matrix4x4[4][4];

template <class TYPE>
class list 
{
public:
	list()
	{
		DataCount = 0; DataList = NULL;
	};

	~list()
	{
		delete [] DataList;
		DataList = NULL;
		DataCount = 0;
	};

	list<TYPE> (list<TYPE> & l );

	TYPE & operator [] (const unsigned int Index);
	list<TYPE> & operator = (list<TYPE> & l);
	
	bool Is_Empty_List ();
	void Add_To_List(TYPE v);
	unsigned int Get_Size();
	void Set_Empty();
	TYPE GetAt(unsigned int Index);
	void Resize(unsigned int NumElems);
	
	TYPE *DataList;
	int DataCount;
};

template <class TYPE>
void list<TYPE>::Resize(unsigned int NumElems)
{
	if ( NumElems == 0 )
	{
		if(DataList != 0)
		{
			delete [] DataList;
			DataCount = 0;
		}
	}

	if ( DataCount == 0)
	{	
		DataList=(TYPE*)malloc(NumElems*sizeof(TYPE));
		memset(DataList, 0, NumElems*sizeof(TYPE));	
	}
	else
	{
		DataList=(TYPE*)realloc(DataList, NumElems*sizeof(TYPE));
	}

	DataCount = NumElems;
}

template <class TYPE>
void list<TYPE>::Set_Empty()
{
	if(DataList != NULL)
	{
		delete [] DataList;
		DataList = NULL;
	}

	DataCount = 0;
}

template <class TYPE>
TYPE list<TYPE>::GetAt(unsigned int Index)
{
	return DataList[Index];
}

template <class TYPE>
list<TYPE>::list (list<TYPE> & l)
{
	DataList = NULL;
	DataCount = 0;

	DataCount = l.DataCount;

	DataList=(TYPE*)malloc(DataCount*sizeof(TYPE));

	memcpy(&DataList[0], &l[0], DataCount*sizeof(TYPE));

	/*
	for ( int i = 0; i < DataCount; i++ )
	{
		//DataList[i] = l[i];
		//DataList[i] = l.GetAt(i);
		
		//DataList[i].x = l[i].x;
		//DataList[i].y = l[i].y;
		//DataList[i].z = l[i].z;
		
	}
	*/
}


template <class TYPE>
list<TYPE> & list<TYPE>::operator = (list<TYPE> & l)
{
	if(DataList != NULL)
	{
		delete [] DataList;
		DataList = NULL;
		DataCount = 0;
	}

	DataCount = l.DataCount;

	DataList=(TYPE*)malloc(DataCount*sizeof(TYPE));

	memcpy(&DataList[0], &l[0], DataCount*sizeof(TYPE));

	/*
	for ( int i = 0; i < DataCount; i++ )
	{
		//DataList[i] = l[i];
		//DataList[i] = l.GetAt(i);
		
		//DataList[i].x = l[i].x;
		//DataList[i].y = l[i].y;
		//DataList[i].z = l[i].z;
		
	}
	*/
	return *this;
}

template <class TYPE>
TYPE & list<TYPE>::operator [] (const unsigned int Index)
{
	return DataList[Index];
}

template <class TYPE>
unsigned int list<TYPE>::Get_Size()
{
	return DataCount;
}

template <class TYPE>
bool list<TYPE>::Is_Empty_List ()
{
		if(DataCount == 0)
			return true;

		return false;
}

template <class TYPE>
void list<TYPE>::Add_To_List(TYPE v)
{
	if ( DataCount == 0)
	{	
		DataCount++;
		DataList=(TYPE*)malloc(DataCount*sizeof(TYPE));
	}
	else
	{
		DataCount++;
		DataList=(TYPE*)realloc(DataList, DataCount*sizeof(TYPE));
	}

	DataList[DataCount-1] = v;
}

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();
	
	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager () ;

private:

	void Create_BackBuffer();
	void Delete_BackBuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();

	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);
	vector3 MidPoint(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Normalize(vector3 &VecIn);

	HWND m_hWnd;

	int m_VertCount;
	int m_TriangleCount;

	//контекст окна приложения
	HDC m_hDC;
	//размеры окна приложения
	UINT m_ViewWidth;
	UINT m_ViewHeight;
	//наш задний буфер
	HDC m_hBackBuffer;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	//кисть для очистки заднего буфера (окна приложения)
	HBRUSH m_hBrush;
	//область для очистки заднего буфера (окна приложения)
	RECT m_Rc;

	list <vector3> m_VertBuff;
	list <vector3> m_VertBuffTransformed;
	list <unsigned int> m_IndexBuff;



};

#endif