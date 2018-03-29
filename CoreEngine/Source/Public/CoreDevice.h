#pragma once

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class CoreDevice
{
public:
	CoreDevice();
	~CoreDevice();

	HRESULT CreateDeviceResources();
	HRESULT CreateWindowResources(HWND hwnd);

	HRESULT ConfigureBackBuffer();
	HRESULT ReleaseBackBuffer();
	HRESULT GoFullScreen();
	HRESULT GoWindowed();

	float GetAspectRatio();

	ID3D11Device*           GetDevice()        { return core_pDevice.Get(); };
	ID3D11DeviceContext*    GetDeviceContext() { return core_pContext.Get(); };
	ID3D11RenderTargetView* GetRenderTarget()  { return core_pRTV.Get(); }
	ID3D11DepthStencilView* GetDepthStencil()  { return core_pDSV.Get(); }
	D3D_FEATURE_LEVEL       GetFeatureLevel()  { return core_eFeatureLevel; }

	void Present();

private:

	//-----------------------------------------------------------------------------
	// Direct3D device
	//-----------------------------------------------------------------------------
	ComPtr<ID3D11Device>        core_pDevice;
	ComPtr<ID3D11DeviceContext> core_pContext; // immediate context
	

	//-----------------------------------------------------------------------------
	// DXGI swap chain device resources
	//-----------------------------------------------------------------------------
	ComPtr<IDXGISwapChain>			core_pSwapChain;
	ComPtr<ID3D11Texture2D>			core_pBackBuffer;
	ComPtr<ID3D11RenderTargetView>	core_pRTV; 


	//-----------------------------------------------------------------------------
	// Direct3D device resources for the depth stencil
	//-----------------------------------------------------------------------------
	ComPtr<ID3D11Texture2D>         core_pDepthStencilBuffer;
	ComPtr<ID3D11DepthStencilView>  core_pDSV;


	//-----------------------------------------------------------------------------
	// Direct3D device metadata and device resource metadata
	//-----------------------------------------------------------------------------
	D3D_FEATURE_LEVEL       core_eFeatureLevel;
	D3D11_TEXTURE2D_DESC    core_sBackBufferDesc;
	D3D11_VIEWPORT          core_sViewport;
};

