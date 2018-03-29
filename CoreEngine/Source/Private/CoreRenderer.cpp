#include "CoreRenderer.h"
#include "CoreUtils.h"
#include <ppltasks.h>


using namespace std;
using namespace DirectX;

CoreRenderer::CoreRenderer(shared_ptr<CoreDevice> coreDevice)
	:core_frameCount(0),
	 coreDevice(coreDevice)
{
	core_frameCount = 0; // init frame count
}

CoreRenderer::~CoreRenderer()
{
	// ComPtr will clean up references for us. But be careful to release
	// references to anything you don't need whenever you call Flush or Trim.
	// As always, clean up your system (CPU) memory resources before exit.
}

void CoreRenderer::CreateDeviceDependentResources()
{
	// Compile shaders using the Effects library.
	auto CreateShadersTask = Concurrency::create_task(
		[this]()
	{
		CreateShaders();
	}
	);

	// Load the geometry for the spinning cube.
	auto CreateCubeTask = CreateShadersTask.then(
		[this]()
	{
		CreateCube();
	}
	);
}

void CoreRenderer::CreateWindowSizeDependentResources()
{
	// Create the view matrix and the perspective matrix.
	CreateViewAndPerspective();
}

void CoreRenderer::Update()
{
	// Rotate the cube 1 degree per frame.
	XMStoreFloat4x4(
		&core_constantBufferData.world,
		XMMatrixTranspose(
			XMMatrixRotationY(
				XMConvertToRadians(
				(float)core_frameCount++
				)
			)
		)
	);

	if (core_frameCount == MAXUINT)  core_frameCount = 0;
}


void CoreRenderer::Render()
{
	// Use the Direct3D device context to draw.
	ID3D11DeviceContext* context = coreDevice->GetDeviceContext();

	ID3D11RenderTargetView* renderTarget = coreDevice->GetRenderTarget();
	ID3D11DepthStencilView* depthStencil = coreDevice->GetDepthStencil();

	context->UpdateSubresource(
		core_pConstantBuffer.Get(),
		0,
		nullptr,
		&core_constantBufferData,
		0,
		0
	);

	// Clear the render target and the z-buffer.
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	context->ClearRenderTargetView(
		renderTarget,
		teal
	);
	context->ClearDepthStencilView(
		depthStencil,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set the render target.
	context->OMSetRenderTargets(
		1,
		&renderTarget,
		depthStencil
	);

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

	context->IASetVertexBuffers(
		0,
		1,
		core_pVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		core_pIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	context->IASetInputLayout(core_pInputLayout.Get());

	// Set up the vertex shader stage.
	context->VSSetShader(
		core_pVertexShader.Get(),
		nullptr,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		core_pConstantBuffer.GetAddressOf()
	);

	// Set up the pixel shader stage.
	context->PSSetShader(
		core_pPixelShader.Get(),
		nullptr,
		0
	);

	// Calling Draw tells Direct3D to start sending commands to the graphics device.
	context->DrawIndexed(
		core_indexCount,
		0,
		0
	);
}

HRESULT CoreRenderer::CreateShaders()
{
	HRESULT hr = S_OK;

	// Use the Direct3D device to load resources into graphics memory.
	ID3D11Device* device = coreDevice->GetDevice();

	// You'll need to use a file loader to load the shader bytecode. In this
	// example, we just use the standard library.
	FILE* vShader, *pShader;
	BYTE* bytes;

	size_t destSize = 32768;
	size_t bytesRead = 0;
	bytes = new BYTE[destSize];

	fopen_s(&vShader, "Shaders/Compiled/CoreVertexShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, destSize, vShader);

	hr = device->CreateVertexShader(
		bytes,
		bytesRead,
		nullptr,
		&core_pVertexShader
	);
	 
	LOG_STR_1("VERTEX SHADER BYTES READ: %d \n", bytesRead)
	 
	D3D11_INPUT_ELEMENT_DESC iaDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes,
		bytesRead,
		&core_pInputLayout
	);

	delete bytes;


	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "Shaders/Compiled/CorePixelShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, destSize, pShader);
	hr = device->CreatePixelShader(
		bytes,
		bytesRead,
		nullptr,
		core_pPixelShader.GetAddressOf()
	);

	LOG_STR_1("PIXEL SHADER BYTES READ: %d \n", bytesRead)

	delete bytes;

	CD3D11_BUFFER_DESC cbDesc(
		sizeof(ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc,
		nullptr,
		core_pConstantBuffer.GetAddressOf()
	);

	fclose(vShader);
	fclose(pShader);


	// Load the extended shaders with lighting calculations.
	/* 
	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&vShader, "CubeVertexShaderLighting.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, vShader);
	hr = device->CreateVertexShader(
	bytes,
	bytesRead,
	nullptr,
	&core_pVertexBuffer
	);

	D3D11_INPUT_ELEMENT_DESC iaDescExtended[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
	0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
	0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
	0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
	iaDesc,
	ARRAYSIZE(iaDesc),
	bytes,
	bytesRead,
	&core_pInputLayoutExtended
	);

	delete bytes;


	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "CubePixelShaderPhongLighting.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, pShader);
	hr = device->CreatePixelShader(
	bytes,
	bytesRead,
	nullptr,
	core_pPixelShader.GetAddressOf()
	);

	delete bytes;

	fclose(vShader);
	fclose(pShader);


	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "CubePixelShaderTexelLighting.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, pShader);
	hr = device->CreatePixelShader(
	bytes,
	bytesRead,
	nullptr,
	core_pPixelShader.GetAddressOf()
	);

	delete bytes;

	fclose(pShader);
	*/ 

	return hr;
}

HRESULT CoreRenderer::CreateCube()
{
	HRESULT hr = S_OK;

	// Use the Direct3D device to load resources into graphics memory.
	ID3D11Device* device = coreDevice->GetDevice();

	// Create cube geometry.
	VertexPositionColor CubeVertices[] =
	{
		{ XMFLOAT3(-0.5f,-0.5f,-0.5f), XMFLOAT3(0,   0,   0), },
		{ XMFLOAT3(-0.5f,-0.5f, 0.5f), XMFLOAT3(0,   0,   1), },
		{ XMFLOAT3(-0.5f, 0.5f,-0.5f), XMFLOAT3(0,   1,   0), },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0,   1,   1), },

		{ XMFLOAT3(0.5f,-0.5f,-0.5f), XMFLOAT3(1,   0,   0), },
		{ XMFLOAT3(0.5f,-0.5f, 0.5f), XMFLOAT3(1,   0,   1), },
		{ XMFLOAT3(0.5f, 0.5f,-0.5f), XMFLOAT3(1,   1,   0), },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1,   1,   1), },
	};

	// Create vertex buffer:
	CD3D11_BUFFER_DESC vDesc(
		sizeof(CubeVertices),
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = CubeVertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&core_pVertexBuffer
	);

	// Create index buffer:
	unsigned short CubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};

	core_indexCount = ARRAYSIZE(CubeIndices);

	CD3D11_BUFFER_DESC iDesc(
		sizeof(CubeIndices),
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = CubeIndices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&iDesc,
		&iData,
		&core_pIndexBuffer
	);

	return hr;
}

void CoreRenderer::CreateViewAndPerspective()
{
	// Use DirectXMath to create view and perspective matrices.

	XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
	XMVECTOR eye = XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
	XMVECTOR at  = XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);

	XMStoreFloat4x4(
		&core_constantBufferData.view,
		XMMatrixTranspose(
		XMMatrixLookAtRH(
				eye,
				at,
				up
			)
		)
	);

	float aspectRatio = coreDevice->GetAspectRatio();

	XMStoreFloat4x4( 
		&core_constantBufferData.projection,
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
