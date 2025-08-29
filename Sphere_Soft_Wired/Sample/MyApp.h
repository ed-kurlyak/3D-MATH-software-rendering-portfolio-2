//======================================================================================
//	Ed Kurlyak 2020 Sphere Mesh Software
//======================================================================================

#ifndef _MYAPP_
#define _MYAPP_

#include <windows.h>
#include <windowsx.h>
#include "MeshManager.h"

#define APPNAME "Sample 3D Application - Ed Kurlyak"
#define CLASSNAME "Sample"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class CMyApp
{
public:
	
	int ProgramBegin(HINSTANCE	hInstance, int nCmdShow);

private:

	HWND m_hWnd;

	static LRESULT CALLBACK StaticWndProc(	HWND hWnd, UINT	uMsg, WPARAM	wParam,	LPARAM	lParam);
	LRESULT WndProc(	HWND hWnd, UINT	uMsg, WPARAM	wParam,	LPARAM	lParam);
	
	CMeshManager MeshManager;

};

#endif