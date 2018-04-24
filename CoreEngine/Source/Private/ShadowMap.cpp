#include "ShadowMap.h"
#include "CoreUtils.h"
#include <iostream>



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
	texDesc.Format				= DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.Width				= mWidth;
	texDesc.Height				= mHeight;
	texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE
								| D3D11_BIND_DEPTH_STENCIL;
	texDesc.ArraySize			= 1;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MipLevels			= 1;
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags			= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;

	ComPtr<ID3D11Texture2D> depthMapTexture = 0;

	HR(device->CreateTexture2D(
		&texDesc,
		0,
		depthMapTexture.GetAddressOf()
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
		depthMapTexture.Get(),
		&dsvDesc,
		mDSV.GetAddressOf()
	));
//================================================================



//================================================================
//	CREATE SHADER RESOURCE VIEW TO BIND TO TEXTURE 
//================================================================
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format						= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels			= texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip	= 0;

	HR(device->CreateShaderResourceView(
		depthMapTexture.Get(),
		&srvDesc,
		mSRV.GetAddressOf()
	));
	
//================================================================



//================================================================
//	CREATE VERTEX SHADER & ITS INPUT LAYOUT
//================================================================
{
	FILE *vShader;
	BYTE *bytes;

	size_t destSize = 32768;
	size_t bytesRead = 0;
	bytes = new BYTE[destSize];

	fopen_s(&vShader, "Shaders/Compiled/ShadowMapShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, destSize, vShader);

	HR(device->CreateVertexShader(
		bytes,
		bytesRead,
		nullptr,
		mVertexShader.GetAddressOf()
	));

	LOG_STR_1("VERTEX SHADER BYTES READ: %d \n", bytesRead);

	
	D3D11_INPUT_ELEMENT_DESC iaDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HR(device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes,
		bytesRead,
		mInputLayout.GetAddressOf()
	));

	delete bytes;
	fclose(vShader);
}
//================================================================



//================================================================
//	CREATE PIXEL SHADER 
//================================================================
{
	FILE *pShader;
	BYTE *bytes;

	size_t destSize = 32768;
	size_t bytesRead = 0;
	bytes = new BYTE[destSize];

	bytesRead = 0;
	fopen_s(&pShader, "Shaders/Compiled/ShadowMapShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, destSize, pShader);
	HR(device->CreatePixelShader(
		bytes,
		bytesRead,
		nullptr,
		mPixelShader.GetAddressOf()
	));

	LOG_STR_1("PIXEL SHADER BYTES READ: %d \n", bytesRead);

	delete bytes;

	fclose(pShader);
}
//================================================================	

	CD3D11_BUFFER_DESC cbDesc(
		sizeof(ShadowMapConstantBuffer),
		D3D11_BIND_CONSTANT_BUFFER
	);

	HR(device->CreateBuffer(
		&cbDesc,
		nullptr,
		mConstantBuffer0.GetAddressOf()
	));

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	HR(device->CreateSamplerState(
		&samplerDesc,
		mSamplerState.GetAddressOf()
	));


	bIsInitialized = true;


}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::BindResources(ID3D11DeviceContext *context)
{
	if (!bIsInitialized)
		return;

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

