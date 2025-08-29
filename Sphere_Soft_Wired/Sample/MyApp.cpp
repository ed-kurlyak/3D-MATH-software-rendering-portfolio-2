//======================================================================================
//	Ed Kurlyak 2020 Sphere Mesh Software
//======================================================================================

#include "MyApp.h"

int WINAPI WinMain(	HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpCmdLine,
					int			nCmdShow)
{
	CMyApp g_App;

	return  g_App.ProgramBegin(hInstance, nCmdShow);
}

LRESULT CALLBACK CMyApp::StaticWndProc(	HWND hWnd,
							UINT msg,
							WPARAM wParam,
							LPARAM lParam)
{

	if ( msg == WM_CREATE )
		SetWindowLong( hWnd, GWL_USERDATA, (LONG)(LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	CMyApp *MainWndProc = (CMyApp*)(LONG_PTR)GetWindowLong( hWnd, GWL_USERDATA );
	if (MainWndProc) return MainWndProc->WndProc( hWnd, msg, wParam, lParam );
	
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

LRESULT CMyApp::WndProc(HWND hWnd, UINT uMsg,	WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int CMyApp::ProgramBegin(HINSTANCE	hInstance, int nCmdShow)
{
	WNDCLASS wcl;

	wcl.style			= CS_HREDRAW | CS_VREDRAW;;
	wcl.lpfnWndProc		= (WNDPROC) StaticWndProc;
	wcl.cbClsExtra		= 0L;
	wcl.cbWndExtra		= 0L;
	wcl.hInstance		= GetModuleHandle(NULL);
	wcl.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wcl.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcl.lpszMenuName	= NULL;
	wcl.lpszClassName	= CLASSNAME;
	
	if(!RegisterClass (&wcl)) return 0;

	m_hWnd = CreateWindow(CLASSNAME, APPNAME,
			  WS_OVERLAPPEDWINDOW,
              0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
              NULL, NULL, hInstance, this);

	if (!m_hWnd) return 0;
	
	RECT window_rect = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};

	AdjustWindowRectEx(&window_rect,
		GetWindowStyle(m_hWnd),
		GetMenu(m_hWnd) != NULL,
		GetWindowExStyle(m_hWnd));

	UINT nWidthScreen = GetSystemMetrics(SM_CXSCREEN);
	UINT nHeightScreen = GetSystemMetrics(SM_CYSCREEN);

	UINT nWidthX = window_rect.right - window_rect.left;
	UINT nWidthY = window_rect.bottom - window_rect.top;

	UINT nPosX =  (nWidthScreen - nWidthX)/2;
	UINT nPosY =  (nHeightScreen - nWidthY)/2;
	
	MoveWindow(m_hWnd,
		nPosX,
        nPosY,
        nWidthX,
        nWidthY,
        FALSE);

	ShowWindow (m_hWnd, nCmdShow);
	UpdateWindow (m_hWnd);
	SetForegroundWindow(m_hWnd);

	MeshManager.InitSphere(m_hWnd);
	
	MSG         msg ;
    while (TRUE)
    {
	    if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
		    if (msg.message == WM_QUIT)
				break;

			TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
		}

		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;
        
		MeshManager.CalculateSphere();
        MeshManager.DrawSphere (m_hWnd) ;
		Sleep(50);
	}

	DestroyWindow(m_hWnd);
	UnregisterClass("Sample", hInstance);

	return (int) msg.wParam;
}



