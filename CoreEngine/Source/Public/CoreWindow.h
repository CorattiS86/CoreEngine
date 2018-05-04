#pragma once

#include <windows.h>
#include <string>

#include "CoreDevice.h"
#include "CoreDrawer.h"
#include "CoreUtils.h"

#define MAX_LOADSTRING 20

using namespace std;

class CoreWindow
{
public:

	CoreWindow();
	~CoreWindow();

	HRESULT CreateDesktopWindow();

	HWND GetWindowHandle() { return core_hWindow; };
	WCHAR* GetWindowTitle() { return core_WindowTitle;  }

	HRESULT Run(
		shared_ptr<CoreDevice> coreDevice,
		shared_ptr<CoreRenderer> coreRenderer
	);

	static LRESULT CALLBACK CoreWindowProcedure(
		HWND hwnd, 
		UINT uMsg, 
		WPARAM wParam, 
		LPARAM lParam
	);

private:
	//-----------------------------------------------------------------------------
	// Desktop window resources
	//-----------------------------------------------------------------------------
	HMENU     core_hMenu;
	RECT      core_Rect;
	HWND      core_hWindow;
	HINSTANCE core_hInstance;
	
	WNDCLASS  coreWindowClass;
	WCHAR core_WindowClassName[MAX_LOADSTRING];                  // The title bar text
	WCHAR core_WindowTitle[MAX_LOADSTRING];            // the main window class name
};
