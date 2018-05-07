/**
*
*	CoreEngine
*
*/

#ifndef UNICODE
#define UNICODE
#endif

/*
*-----------------------
*  To enable debug log
*-----------------------
*/

#include "stdafx.h"
#include "resource.h"

#include <memory>

#include "CoreDevice.h"
#include "CoreRenderer.h"
#include "CoreWindow.h"
#include "CoreUtils.h"
#include "MonkeyBuilder.h"
#include "PlaneBuilder.h"

using namespace std;


//-----------------------------------------------------------------------------
// Main function: Creates window, calls initialization functions, and hosts
// the render loop.
//-----------------------------------------------------------------------------

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{		
	HRESULT hr = S_OK;
	
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Begin initialization.
	LOG("INFO: begin initialization... \n")
	// Instantiate the window manager class.
	shared_ptr<CoreWindow> coreWindow = shared_ptr<CoreWindow>(new CoreWindow());
	LOG("INFO: CoreWindow instantiated !!! \n")

	// Create a window.
	hr = coreWindow->CreateDesktopWindow();
	if (FAILED(hr)) {
		LOG("ERROR: creation desktop window failed !!! \n")
			return hr;
	}
	LOG("INFO: creation desktop window succeded !!! \n")
	
	if (SUCCEEDED(hr))
	{
		// Instantiate the device manager class.
		shared_ptr<CoreDevice> coreDevice = shared_ptr<CoreDevice>(new CoreDevice());
		LOG("INFO: CoreDevice instantiated !!! \n")
		// Create device resources.

		hr = coreDevice->CreateDeviceResources();
		
		shared_ptr<CoreRenderer> coreRenderer   = shared_ptr<CoreRenderer>(new CoreRenderer(coreDevice));

		LOG("INFO: CoreRenderer instantiated !!! \n")
		
		// We have a window, so initialize window size-dependent resources.
		hr = coreDevice->CreateWindowResources(coreWindow->GetWindowHandle());

		// Go full-screen.
		//hr = coreDevice->GoFullScreen();

		shared_ptr<CoreCamera> coreCamera = shared_ptr<CoreCamera>(new CoreCamera(coreDevice));
		coreCamera->setUpDirection(0.0f, 1.0f, 0.0f);
		coreCamera->setEyePosition(0.0f, 20.0f, 20.0f);
		coreCamera->setLookAt(0.0f, 0.0f, 0.0f);
		coreCamera->setPerspectiveProjection(coreDevice->GetAspectRatio());


		MonkeyBuilder monkeyBuilder(coreDevice);
		CoreRenderable * Monkey = monkeyBuilder.buildRenderable();
		
		PlaneBuilder planeBuilder(coreDevice);
		CoreRenderable * Plane = planeBuilder.buildRenderable();
		Plane->TranslateCoordinates(0.0f, -2.0f, 0.0f);

		coreRenderer->SetCamera(coreCamera);

		vector<CoreRenderable>		mRenderables;
		mRenderables.push_back(*Monkey);
		mRenderables.push_back(*Plane);

		hr = coreWindow->Run(coreDevice, coreRenderer, coreCamera, mRenderables);

	}

	// Cleanup is handled in destructors.
	return hr;
}