#pragma once

#include "CoreRenderable.h"
#include "CoreDevice.h"
#include "CoreCommon.h"
#include "CoreUtils.h"
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <ppltasks.h>
#include "DDSTextureLoader.h"
#include "Object.h"

using namespace std;
using namespace DirectX;

class PlaneBuilder : public CoreRenderableBuilder
{
public:
	PlaneBuilder(shared_ptr<CoreDevice> coreDevice)
		: mCoreDevice(coreDevice)
	{
		Init();
	}

private:
	void Init();
	void LoadShaders();
	void LoadObject(const char * filename);
	void LoadTexture(const wchar_t * filename);
	void LoadGPUResource();
	void LoadState();

private:

	shared_ptr<CoreDevice> mCoreDevice;
};



void PlaneBuilder::Init()
{
	LoadShaders();

	LoadObject("Resources/Plane/Plane.obj");

	LoadTexture(L"Resources/Plane/PlaneDiffuse.dds");

	LoadGPUResource();

	LoadState();

}

void PlaneBuilder::LoadShaders()
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();

	// You'll need to use a file loader to load the shader bytecode. In this
	// example, we just use the standard library.
	FILE* vShader, *pShader;
	BYTE* bytes;

	size_t destSize = 65356;
	size_t bytesRead = 0;
	bytes = new BYTE[destSize];

	fopen_s(&vShader, "Shaders/Compiled/CoreVertexShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, destSize, vShader);

	HR(device->CreateVertexShader(
		bytes,
		bytesRead,
		nullptr,
		&_VertexShader
	));

	LOG_STR_1("VERTEX SHADER BYTES READ: %d \n", bytesRead)

		D3D11_INPUT_ELEMENT_DESC iaDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HR(device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes,
		bytesRead,
		&_InputLayout
	));

	delete bytes;

	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "Shaders/Compiled/CorePixelShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, destSize, pShader);

	HR(device->CreatePixelShader(
		bytes,
		bytesRead,
		nullptr,
		&_PixelShader
	));

	LOG_STR_1("PIXEL SHADER BYTES READ: %d \n", bytesRead)

		delete bytes;

	CD3D11_BUFFER_DESC cbDesc(
		sizeof(XMFLOAT4X4),
		D3D11_BIND_CONSTANT_BUFFER
	);

	HR(device->CreateBuffer(
		&cbDesc,
		nullptr,
		&_WorldConstantBuffer
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
		&_SamplerState
	));

	fclose(vShader);
	fclose(pShader);
}

void PlaneBuilder::LoadObject(const char * filename)
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();

	Object obj;
	obj.LoadTexObjectFromFile(filename);

	// Create vertex buffer:
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
		&_VertexBuffer
	));

	_VerticesCount = obj.getTexVerticesCount();
	_IsWithIndices = false;

	_Stride = sizeof(*(obj.getTexVertices()));
	_Offset = 0;

	_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	XMStoreFloat4x4(
		&_WorldCoordinatesMatrix,
		XMMatrixIdentity()
	);
}

void PlaneBuilder::LoadTexture(const wchar_t * filename)
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	ComPtr<ID3D11Texture2D> texture;
	CreateDDSTextureFromFile(
		device,
		context,
		filename,
		(ID3D11Resource**)texture.GetAddressOf(),
		_SRV.GetAddressOf()
	);
}

void PlaneBuilder::LoadGPUResource()
{
	_RTV = mCoreDevice->GetRenderTargetView();
	_DSV = mCoreDevice->GetDepthStencilView();
	_Viewport = mCoreDevice->GetViewport();

	float teal[] = { 0.098f, 0.439f, 0.439f, 1.0f };
	_BackgroundColor[0] = teal[0];
	_BackgroundColor[1] = teal[1];
	_BackgroundColor[2] = teal[2];


}

void PlaneBuilder::LoadState()
{
	ID3D11Device		 *device = mCoreDevice->GetDevice();
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	D3D11_RASTERIZER_DESC rsDesc;

	//ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	//rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	//rsDesc.CullMode = D3D11_CULL_NONE;
	//rsDesc.FrontCounterClockwise = true;
	//rsDesc.DepthClipEnable = true;

	//device->CreateRasterizerState(&rsDesc, _RasterizerState.GetAddressOf());

	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.DepthClipEnable = true;

	device->CreateRasterizerState(&rsDesc, _RasterizerState.GetAddressOf());


}