#include "ShadowMap.h"
#include "CoreUtils.h"
#include <iostream>
#include <ScreenGrab.h>


ShadowMap::ShadowMap(
	ID3D11Device * device, 
	UINT width, 
	UINT height
)	
	: mWidth(width)
	, mHeight(height)
	, bIsInitialized(false)
{

//================================================================
//	DEFINE THE VIEWPORT
//================================================================
	ZeroMemory(&mViewport, sizeof(mViewport));
	mViewport.Width		= static_cast<float>(mWidth);
	mViewport.Height	= static_cast<float>(mHeight);
	mViewport.TopLeftX	= 0.0f;
	mViewport.TopLeftY	= 0.0f;
	mViewport.MinDepth	= 0.0f;
	mViewport.MaxDepth	= 1.0f;
//================================================================



//================================================================
// CREATE THE TEXTURE TO USE AS SHADOW MAP 
//================================================================
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.Width				= mWidth;
	texDesc.Height				= mHeight;
	texDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL
;
	texDesc.ArraySize			= 1;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MipLevels			= 1;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags			= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;

	HR(device->CreateTexture2D(
		&texDesc,
		0,
		mDepthMapTexture.GetAddressOf()
	));
//================================================================



//================================================================
//	CREATE DEPTH STENCIL VIEW TO BIND TO TEXTURE 
//================================================================
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Flags				= 0;
	dsvDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice	= 0;

	HR(device->CreateDepthStencilView(
		mDepthMapTexture.Get(),
		&dsvDesc,
		mDSV.GetAddressOf()
	));
//================================================================



//================================================================
//	CREATE SHADER RESOURCE VIEW TO BIND TO TEXTURE 
//================================================================
	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//ZeroMemory(&srvDesc, sizeof(srvDesc));
	//srvDesc.Format						= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	//srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels			= texDesc.MipLevels;
	//srvDesc.Texture2D.MostDetailedMip	= 0;

	//HR(device->CreateShaderResourceView(
	//	mDepthMapTexture.Get(),
	//	&srvDesc,
	//	mSRV.GetAddressOf()
	//));
	
//================================================================

//================================================================
//	CREATE CONSTANT BUFFER
//================================================================
	CD3D11_BUFFER_DESC cbDesc(
		sizeof(ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	HR(device->CreateBuffer(
		&cbDesc,
		nullptr,
		mConstantBuffer.GetAddressOf()
	));

	//================================================================

	bIsInitialized = true;
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::BindResources(ID3D11DeviceContext *context)
{
	if (!bIsInitialized)
		return;

	context->UpdateSubresource(
		mConstantBuffer.Get(),
		0,
		nullptr,
		&mShadowMapConstantBuffer,
		0,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		mConstantBuffer.GetAddressOf()
	);

	context->RSSetViewports(
		1,
		&mViewport
	);

	ID3D11RenderTargetView * renderTargets[1] = { 0 };

	context->OMSetRenderTargets(
		1,
		renderTargets,
		mDSV.Get()
	);

	context->ClearDepthStencilView(
		mDSV.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f, 
		0
	);

}

void ShadowMap::SetPointOfView(
	XMVECTOR up, XMVECTOR eye, XMVECTOR at
)
{
	XMStoreFloat4x4(
		&mShadowMapConstantBuffer.world,
		XMMatrixIdentity()
	);

	XMStoreFloat4x4(
		&mShadowMapConstantBuffer.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				eye,
				at,
				up
			)
		)
	);

	float aspectRatio = static_cast<float>(mWidth) / static_cast<float>(mHeight);

	XMStoreFloat4x4(
		&mShadowMapConstantBuffer.projection,
		XMMatrixTranspose(
			XMMatrixPerspectiveFovRH(
				XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);
}

void ShadowMap::Capture(ID3D11DeviceContext	*context)
{
	//================================================================
	//	SAVE TEXTUREs
	//================================================================
	
	SaveDDSTextureToFile(
		context,
		mDepthMapTexture.Get(),
		L"Snapshots/ShadowMapTexture.dds"
	);
	//================================================================
}

