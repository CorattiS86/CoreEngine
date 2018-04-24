#include "RenderableObject.h"
#include "CoreUtils.h"
#include <iostream>



RenderableObject::RenderableObject(
	shared_ptr<CoreDevice> coreDevice,
	UINT width,
	UINT height
)
	: mWidth(width)
	, mHeight(height)
	, mCoreDevice(coreDevice)
{
	mViewport	= coreDevice->GetViewport();
	mRTV		= coreDevice->GetRenderTarget();
	mDSV		= coreDevice->GetDepthStencil();
}

RenderableObject::~RenderableObject()
{
}


void RenderableObject::CreateVertexBuffer(const char * filename)
{
	ID3D11Device* device = mCoreDevice->GetDevice();

//================================================================
//	LOAD OBJECT FROM FILE
//================================================================

	Object obj;
	obj.LoadTexObjectFromFile(filename);

	mStride = sizeof(*(obj.getTexVertices()));
	mOffset = 0;
//================================================================



//================================================================
//	CREATE VERTEX BUFFER
//================================================================

	CD3D11_BUFFER_DESC vDesc(
		sizeof(*(obj.getTexVertices())) * obj.getTexVerticesCount(),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_IMMUTABLE
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = obj.getTexVertices();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(
		&vDesc,
		&vData,
		&mVertexBuffer
	));

	mVerticesCount = obj.getTexVerticesCount();
	bIsWithIndices = false;
//================================================================

}

void RenderableObject::SetShaders()
{
	ID3D11Device * device = mCoreDevice->GetDevice();

//================================================================
//	CREATE VERTEX SHADER & ITS INPUT LAYOUT
//================================================================
	{
		FILE *vShader;
		BYTE *bytes;

		size_t destSize = 32768;
		size_t bytesRead = 0;
		bytes = new BYTE[destSize];

		fopen_s(&vShader, "Shaders/Compiled/TestShader.cso", "rb");
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
			{ 
				"POSITION", 
				0, 
				DXGI_FORMAT_R32G32B32_FLOAT,
				0, 
				0, 
				D3D11_INPUT_PER_VERTEX_DATA,
				0 
			},

			{ 
				"COLOR", 
				0, 
				DXGI_FORMAT_R32G32B32_FLOAT,
				0, 
				12, 
				D3D11_INPUT_PER_VERTEX_DATA, 
				0 
			},

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
		fopen_s(&pShader, "Shaders/Compiled/TestShader.cso", "rb");
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



//================================================================
//	CREATE CONSTANT BUFFER
//================================================================
	CD3D11_BUFFER_DESC cbDesc(
		sizeof(constantBufferData),
		D3D11_BIND_CONSTANT_BUFFER
	);

	HR(device->CreateBuffer(
		&cbDesc,
		nullptr,
		mConstantBuffer0.GetAddressOf()
	));
//================================================================



//================================================================
//	CREATE SAMPLER STATE
//================================================================
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias		= 0.0f;
	samplerDesc.MaxAnisotropy	= 1;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD			= -FLT_MAX;
	samplerDesc.MaxLOD			= FLT_MAX;

	HR(device->CreateSamplerState(
		&samplerDesc,
		mSamplerState.GetAddressOf()
	));
//================================================================

}

void RenderableObject::BindResources()
{

}

void RenderableObject::Render()
{
	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mConstantBuffer0.Get(),
		0,
		nullptr,
		&constantBufferData,
		0,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		mConstantBuffer0.GetAddressOf()
	);

	context->VSSetShader(
		mVertexShader.Get(),
		nullptr,
		0
	);

	context->PSSetShader(
		mPixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetSamplers(
		0,
		1,
		mSamplerState.GetAddressOf()
	);

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	context->IASetVertexBuffers(
		0,
		1,
		mVertexBuffer.GetAddressOf(),
		&mStride,
		&mOffset
	);

	if (bIsWithIndices)
	{
		context->IASetIndexBuffer(
			mIndexBuffer.Get(),
			DXGI_FORMAT_R16_UINT,
			0
		);

		context->DrawIndexed(
			mIndicesCount,
			0,
			0
		);
	}
	else
	{
		context->Draw(
			mVerticesCount,
			0
		);
	}
}