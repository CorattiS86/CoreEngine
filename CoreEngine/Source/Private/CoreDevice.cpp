#include "CoreDevice.h"
#include "CoreUtils.h"
#include <string>
#include <memory>
#include <dxgi1_3.h>

CoreDevice::CoreDevice()
{
}


CoreDevice::~CoreDevice()
{
}

HRESULT CoreDevice::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// This flag adds support for surfaces with a color-channel ordering different
	// from the API default. It is required for compatibility with Direct2D.
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device>        device;
	ComPtr<ID3D11DeviceContext> context;

	hr = D3D11CreateDevice(
		nullptr,                    // Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
		0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		deviceFlags,                // Set debug and Direct2D compatibility flags.
		levels,                     // List of feature levels this app can support.
		ARRAYSIZE(levels),          // Size of the list above.
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,                    // Returns the Direct3D device created.
		&core_eFeatureLevel,        // Returns feature level of device created.
		&context                    // Returns the device immediate context.
	);

	LOG_STR_2("FEATURE LEVEL: %d.%d\n", (core_eFeatureLevel >> 3 * 4) & 0x000F, (core_eFeatureLevel >> 2 * 4) & 0x000F)

	if (FAILED(hr))
	{
		// Handle device interface creation failure if it occurs.
		// For example, reduce the feature level requirement, or fail over 
		// to WARP rendering.
	}

	// Store pointers to the Direct3D 11.1 API device and immediate context.
	device.As(&core_pDevice);
	context.As(&core_pContext);

	return hr;
}

HRESULT CoreDevice::CreateWindowResources(HWND hWnd)
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;      //multisampling setting
	desc.SampleDesc.Quality = 0;    //vendor-specific flag
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.OutputWindow = hWnd;

	// Create the DXGI device object to use in other factories, such as Direct2D.
	ComPtr<IDXGIDevice3> dxgiDevice;
	core_pDevice.As(&dxgiDevice);

	// Create swap chain.
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory> factory;

	hr = dxgiDevice->GetAdapter(&adapter);

	if (SUCCEEDED(hr))
	{
		adapter->GetParent(IID_PPV_ARGS(&factory));

		hr = factory->CreateSwapChain(
			core_pDevice.Get(),
			&desc,
			&core_pSwapChain
		);
	}

	// Configure the back buffer, stencil buffer, and viewport.
	hr = ConfigureBackBuffer();

	return hr;
}

HRESULT CoreDevice::ConfigureBackBuffer()
{
	HRESULT hr = S_OK;

	hr = core_pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&core_pBackBuffer);

	hr = core_pDevice->CreateRenderTargetView(
		core_pBackBuffer.Get(),
		nullptr,
		core_pRTV.GetAddressOf()
	);

	core_pBackBuffer->GetDesc(&core_sBackBufferDesc);

	// Create a depth-stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (core_sBackBufferDesc.Width),
		static_cast<UINT> (core_sBackBufferDesc.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	core_pDevice->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&core_pDepthStencilBuffer
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	core_pDevice->CreateDepthStencilView(
		core_pDepthStencilBuffer.Get(),
		&depthStencilViewDesc,
		&core_pDSV
	);

	ZeroMemory(&core_sViewport, sizeof(D3D11_VIEWPORT));
	core_sViewport.TopLeftX = 0.0f;
	core_sViewport.TopLeftY = 0.0f;
	core_sViewport.Height = (float)core_sBackBufferDesc.Height;
	core_sViewport.Width = (float)core_sBackBufferDesc.Width;
	core_sViewport.MinDepth = 0.0f;
	core_sViewport.MaxDepth = 1.0f;

	core_pContext->RSSetViewports(
		1,
		&core_sViewport
	);
	
	return hr;
}

HRESULT CoreDevice::ReleaseBackBuffer()
{
	HRESULT hr = S_OK;

	// Release the render target view based on the back buffer:
	core_pRTV.Reset();

	// Release the back buffer itself:
	core_pBackBuffer.Reset();

	// The depth stencil will need to be resized, so release it (and view):
	core_pDSV.Reset();
	core_pDepthStencilBuffer.Reset();

	// After releasing references to these resources, we need to call Flush() to 
	// ensure that Direct3D also releases any references it might still have to
	// the same resources - such as pipeline bindings.
	core_pContext->Flush();

	return hr;
}

HRESULT CoreDevice::GoFullScreen()
{
	HRESULT hr = S_OK;

	hr = core_pSwapChain->SetFullscreenState(TRUE, NULL);

	// Swap chains created with the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL flag need to
	// call ResizeBuffers in order to realize a full-screen mode switch. Otherwise, 
	// your next call to Present will fail.

	// Before calling ResizeBuffers, you have to release all references to the back 
	// buffer device resource.
	ReleaseBackBuffer();

	// Now we can call ResizeBuffers.
	hr = core_pSwapChain->ResizeBuffers(
		0,                   // Number of buffers. Set this to 0 to preserve the existing setting.
		0, 0,                // Width and height of the swap chain. Set to 0 to match the screen resolution.
		DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer format.
		0
	);

	// Then we can recreate the back buffer, depth buffer, and so on.
	hr = ConfigureBackBuffer();

	return hr;
}

HRESULT CoreDevice::GoWindowed()
{
	HRESULT hr = S_OK;

	hr = core_pSwapChain->SetFullscreenState(FALSE, NULL);

	// Swap chains created with the DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL flag need to
	// call ResizeBuffers in order to realize a change to windowed mode. Otherwise, 
	// your next call to Present will fail.

	// Before calling ResizeBuffers, you have to release all references to the back 
	// buffer device resource.
	ReleaseBackBuffer();

	// Now we can call ResizeBuffers.
	hr = core_pSwapChain->ResizeBuffers(
		0,                   // Number of buffers. Set this to 0 to preserve the existing setting.
		0, 0,                // Width and height of the swap chain. MUST be set to a non-zero value. For example, match the window size.
		DXGI_FORMAT_UNKNOWN, // This tells DXGI to retain the current back buffer format.
		0
	);

	// Then we can recreate the back buffer, depth buffer, and so on.
	hr = ConfigureBackBuffer();

	return hr;
}

float CoreDevice::GetAspectRatio()
{
	return static_cast<float>(core_sBackBufferDesc.Width) / static_cast<float>(core_sBackBufferDesc.Height);
}

void CoreDevice::Present()
{
	core_pSwapChain->Present(1, 0);
}
