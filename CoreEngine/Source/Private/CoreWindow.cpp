#include "CoreWindow.h"
#include "CoreTimer.h"
#include "Resource.h"

using namespace std;

CoreWindow::CoreWindow()
	:core_hMenu(NULL), //NO Menu
	 core_Rect({ 0, 0, 0, 0 }),
	 core_hWindow(NULL),
	 //core_WindowClassName(core_WindowClassName),
	 //core_WindowTitle(windowTitle),
	 core_hInstance(NULL)
{
	if (core_hInstance == NULL)
		core_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	LoadStringW(core_hInstance, IDC_COREENGINE, core_WindowClassName, MAX_LOADSTRING);
	LoadStringW(core_hInstance, IDS_APP_TITLE, core_WindowTitle, MAX_LOADSTRING);
}


CoreWindow::~CoreWindow()
{
}

HRESULT CoreWindow::CreateDesktopWindow()
{
	if (core_hInstance == NULL)
		core_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	// Register the windows class
	coreWindowClass.lpszClassName = core_WindowClassName;
	coreWindowClass.lpfnWndProc = CoreWindowProcedure;
	coreWindowClass.lpszMenuName = NULL;
	coreWindowClass.style = CS_DBLCLKS;
	coreWindowClass.cbClsExtra = 0;
	coreWindowClass.cbWndExtra = 0;
	coreWindowClass.hInstance = core_hInstance;
	coreWindowClass.hIcon = LoadIcon(core_hInstance, MAKEINTRESOURCE(IDI_COREENGINE)); //the custom icon
	coreWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	coreWindowClass.hbrBackground = CreateSolidBrush(RGB(66, 66, 150));

	if (!RegisterClass(&coreWindowClass))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(dwError);
	}

	// Create the window  
	core_hWindow = CreateWindowEx(
		CS_DBLCLKS,                     // Optional window styles.
		coreWindowClass.lpszClassName,  // Window class
		core_WindowTitle,               // Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		0,							    // X LeftUpperCorner Position //CW_USEDEFAULT
		0,							    // Y LeftUpperCorner Position //CW_USEDEFAULT
		800,							// Width  //CW_USEDEFAULT
		600,							// Height //CW_USEDEFAULT
		NULL,							// Parent window    
		NULL,							// Menu
		core_hInstance,					// Instance handle
		NULL							// Additional application data
	);

	if (!core_hWindow)
	{
		return FALSE;
	}

	CoreTimer::getInstance().Start();

	return S_OK;
}

HRESULT CoreWindow::Run(
	std::shared_ptr<CoreDevice> coreDevice, 
	std::shared_ptr<CoreRenderer> coreRenderer,
	std::shared_ptr<CoreCamera> coreCamera,
	std::vector<CoreRenderable> renderables
	
)
{
	HRESULT hr = S_OK;

	if(!IsWindowVisible(core_hWindow))
		ShowWindow(core_hWindow, SW_SHOW);

	// The render loop is controlled here.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Process window events.
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		
		if (bGotMsg)
		{	
			// Translate and dispatch the message
			TranslateMessage(&msg); //translates keystrokes (key down, key up) into characters
			DispatchMessage(&msg);  //tells the OS to call the window procedure of the window that is the target of the message
		}
		else
		{
	
		 
			coreRenderer->RenderAll(renderables);
			 

			coreDevice->Present();

			CoreTimer::getInstance().Tick();
			CoreTimer::getInstance().ShowFrame(this->core_hWindow);
		}
	}

	return hr;
}

LRESULT CALLBACK CoreWindow::CoreWindowProcedure(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		HMENU hMenu;
		hMenu = GetMenu(hWnd);
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(hWnd);
		//UnregisterClass( coreWindowClass, core_hInstance ); // static method, doesn't see core_WindowClassNam and core_hInstance
 
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}