#include "ShadowMap.h"
#include "CoreUtils.h"
#include "ScreenGrab.h"
#include <wrl.h>
#include "DDSTextureLoader.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ShadowMap::ShadowMap(
	shared_ptr<CoreDevice> coreDevice,
	UINT width, 
	UINT height
)
	: mCoreDevice(coreDevice)
	, mWidth(width)
	, mHeight(height)
	, mUpDirection(0.0f, 1.0f, 0.0f, 1.0f)
	, mLightPosition(0.0f, 0.0f, 0.0f, 1.0f)
	, mLookAt(0.0f, 0.0f, 0.0f, 1.0f)
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	ZeroMemory(&mViewport, sizeof(mViewport));
	mViewport.Width		= mWidth;
	mViewport.Height	= mHeight;
	mViewport.TopLeftX	= 0.0f;
	mViewport.TopLeftY	= 0.0f;
	mViewport.MaxDepth	= 1.0f;
	mViewport.MinDepth	= 0.0f;
	
	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going
	// to interpret the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width				= mWidth;
	texDesc.Height				= mHeight;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL 
								| D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	ComPtr<ID3D11Texture2D> depthMap;

	HR(device->CreateTexture2D(
		&texDesc, 
		0, 
		depthMap.GetAddressOf())
	);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	HR(device->CreateDepthStencilView(
		depthMap.Get(), 
		&dsvDesc, 
		mDepthMapDSV.GetAddressOf())
	);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	
	HR(device->CreateShaderResourceView(
		depthMap.Get(), 
		&srvDesc, 
		mDepthMapSRV.GetAddressOf())
	);

	{
		CD3D11_BUFFER_DESC cbDesc(
			sizeof(XMFLOAT4X4),
			D3D11_BIND_CONSTANT_BUFFER
		);

		HR(device->CreateBuffer(
			&cbDesc,
			nullptr,
			mMatrixPositionBuffer.GetAddressOf()
		));
	}

	{
		CD3D11_BUFFER_DESC cbDesc(
			sizeof(ViewProjection),
			D3D11_BIND_CONSTANT_BUFFER
		);

		HR(device->CreateBuffer(
			&cbDesc,
			nullptr,
			mLightViewProjBuffer.GetAddressOf()
		));
	}

	ResetCoordinates();

}

ShadowMap::~ShadowMap()
{
}

ID3D11ShaderResourceView * ShadowMap::DepthMapSRV()
{
	return mDepthMapSRV.Get();
}

ID3D11ShaderResourceView * ShadowMap::PrecomputedDepthMapSRV()
{
	return mPrecomputedDepthMapSRV.Get();
}

void ShadowMap::ResetCoordinates()
{
	XMStoreFloat4x4(
		&mMatrixPosition,
		XMMatrixIdentity()
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext(); 

	context->UpdateSubresource(
		mMatrixPositionBuffer.Get(),
		0,
		nullptr,
		&mMatrixPosition,
		0,
		0
	);
}


void ShadowMap::SetPosition(float x, float y, float z)
{
	mLightPosition = XMFLOAT4(x, y, z, 1.0f);

	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat4(&mLightPosition),
				XMLoadFloat4(&mLookAt),
				XMLoadFloat4(&mUpDirection)
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mLightViewProjBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);
}

void ShadowMap::SetLookAt(float x, float y, float z)
{
	mLookAt = XMFLOAT4(x, y, z, 1.0f);

	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat4(&mLightPosition),
				XMLoadFloat4(&mLookAt),
				XMLoadFloat4(&mUpDirection)
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mLightViewProjBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);
}

void ShadowMap::SetUpDirection(float x, float y, float z)
{
	mUpDirection = XMFLOAT4(x, y, z, 1.0f);

	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat4(&mLightPosition),
				XMLoadFloat4(&mLookAt),
				XMLoadFloat4(&mUpDirection)
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mLightViewProjBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);
}

void ShadowMap::SetPerspectiveProjection(float aspectRatio)
{
	XMStoreFloat4x4(
		&mViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixPerspectiveFovRH(
				XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mLightViewProjBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);
}

void ShadowMap::SetOrthographicProjection()
{
	XMStoreFloat4x4(
		&mViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixOrthographicRH(
				5.0f,
				5.0f,
				0.01f,
				100.0f
			)
		)
	);
	
	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mLightViewProjBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);
}

void ShadowMap::BindDsvAndSetNullRenderTarget()
{
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	context->RSSetViewports(
		1,
		&mViewport
	);

	ID3D11RenderTargetView * renderTargets[] = { 0 };
	context->OMSetRenderTargets(
		1,
		renderTargets,
		mDepthMapDSV.Get()
	);
}

void ShadowMap::Clear()
{
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	context->ClearDepthStencilView(
		mDepthMapDSV.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0
	);
}

void ShadowMap::SetForScreenshot()
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width				= mWidth;
	texDesc.Height				= mHeight;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	HR(device->CreateTexture2D(
		&texDesc, 
		0, 
		mDepthMapTexture.GetAddressOf()
	));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	HR(device->CreateDepthStencilView(
		mDepthMapTexture.Get(),
		&dsvDesc, 
		mDepthMapDSV.GetAddressOf())
	);
}

void ShadowMap::Screenshot()
{
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	SaveDDSTextureToFile(
		context,
		mDepthMapTexture.Get(),
		L"Snapshots/DepthTexture.dds"
	);
}

void ShadowMap::LoadMapping()
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	ComPtr<ID3D11Texture2D> texture;
	HR(CreateDDSTextureFromFile(
		device,
		context,
		L"Snapshots/CubeDiffuse.dds",
		(ID3D11Resource**)texture.GetAddressOf(),
		mPrecomputedDepthMapSRV.GetAddressOf()
	));
}
