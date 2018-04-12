#include "CoreRenderer.h"
#include "CoreUtils.h"
#include <ppltasks.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace DirectX;

CoreRenderer::CoreRenderer(shared_ptr<CoreDevice> coreDevice)
	:core_frameCount(0),
	 coreDevice(coreDevice)
{
	core_frameCount = 0; // init frame count
	ResetWorld();
	
	//AssemblyObject("CubeT.obj");
	//AssemblyAllObject("CubeT.obj");
	AssemblyObject("Resources/Monkey.obj");
	//AssemblyObject("Sphere.obj");
	AssemblyObject("Resources/Plane.obj");
}

CoreRenderer::~CoreRenderer()
{
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

}

void CoreRenderer::CreateWindowSizeDependentResources()
{
	// Create the view matrix and the perspective matrix.
	CreateViewAndPerspective();
}

void CoreRenderer::Update()
{
	core_frameCount++;

	if (core_frameCount >= MAXUINT)  core_frameCount = 0;
}

void CoreRenderer::TranslateWorld(float axisX, float axisY, float axisZ)
{
	XMMATRIX translation =
		XMMatrixTranslation(
			axisX,
			axisY,
			axisZ
		);

	XMStoreFloat4x4(
		&core_constantBufferData.world,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&core_constantBufferData.world),
				translation
				)
		)
	);
}

void CoreRenderer::RotateWorld(float roll, float pitch, float yaw)
{
	XMFLOAT3 InputAngles(roll, pitch, yaw);
	XMVECTOR VectorAngles = XMLoadFloat3(&InputAngles);

	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(VectorAngles);
		
	XMStoreFloat4x4(
		&core_constantBufferData.world,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&core_constantBufferData.world),
				rotation
			)
		)
	);
}

void CoreRenderer::ScaleWorld(float Sx, float Sy, float Sz)
{
	XMFLOAT3 InputScale(Sx, Sy, Sz);
	XMVECTOR VectorAngles = XMLoadFloat3(&InputScale);

	XMMATRIX scale = XMMatrixScalingFromVector(VectorAngles);

	XMStoreFloat4x4(
		&core_constantBufferData.world,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&core_constantBufferData.world),
				scale
			)
		)
	);
}

void CoreRenderer::ResetWorld()
{
	XMStoreFloat4x4(&core_constantBufferData.world, XMMatrixIdentity());
}

void CoreRenderer::UpdateInverseTranspose()
{
	XMStoreFloat4x4(
		&core_normalConstantBufferData.inverse,
		XMMatrixTranspose(
			XMMatrixInverse(
				&XMMatrixDeterminant(
					XMLoadFloat4x4(&core_constantBufferData.world)
				),
				XMLoadFloat4x4(&core_constantBufferData.world)
			)
		)
	);

	XMStoreFloat4x4(
		&core_normalConstantBufferData.transpose,
		XMMatrixTranspose(
			XMMatrixTranspose(
				XMLoadFloat4x4(&core_normalConstantBufferData.inverse)
			)
		)
	);
}

void CoreRenderer::SetStates()
{
	ID3D11Device* device = coreDevice->GetDevice();
	ID3D11DeviceContext* context = coreDevice->GetDeviceContext();

	D3D11_RASTERIZER_DESC rsDesc;

	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.DepthClipEnable = true;

	device->CreateRasterizerState(&rsDesc, core_pRasterStateWireframeMode.GetAddressOf());

	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.DepthClipEnable = true;

	device->CreateRasterizerState(&rsDesc, core_pRasterStateFillMode.GetAddressOf());

	context->RSSetState(core_pRasterStateFillMode.Get());
	//context->RSSetState(0);
}

void CoreRenderer::Render()
{
	// Use the Direct3D device context to draw.
	ID3D11DeviceContext* context = coreDevice->GetDeviceContext();

	ID3D11RenderTargetView* renderTarget = coreDevice->GetRenderTarget();
	ID3D11DepthStencilView* depthStencil = coreDevice->GetDepthStencil();

	ID3D11Device* device = coreDevice->GetDevice();

	// Clear the render target and the z-buffer.
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	const float black[] = { 0.0f, 0.0f, 0.0f, 1.000f };

	const int threshold = 0;
	static int motionBlur = threshold;
	motionBlur++;
	if(motionBlur > threshold) {
		context->ClearRenderTargetView(
			renderTarget,
			teal
		);

		motionBlur = 0;
	}
	
	context->ClearDepthStencilView(
		depthStencil,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	// Set the render target.
	context->OMSetRenderTargets(
		1,
		&renderTarget,
		depthStencil
	);

	context->IASetInputLayout(core_pInputLayout.Get());

	// Set up the vertex shader stage.
	context->VSSetShader(
		core_pVertexShader.Get(),
		nullptr,
		0
	);

	// Set up the pixel shader stage.
	context->PSSetShader(
		core_pPixelShader.Get(),
		nullptr,
		0
	);

	for (auto& obj : vObjectBuffer)
	{
		RenderObjects(&obj);
	}

}

void CoreRenderer::AssemblyObject(const char * filename)
{
	Object obj;
	obj.LoadObjectFromFile(filename);
	obj.SetColor(1.0f, 1.0f, 0.0f);
	CreateObjectBuffer(&obj);
}

void CoreRenderer::AssemblyAllObject(const char * filename)
{
	Object obj;
	obj.LoadAllObjectFromFile(filename);
	obj.SetColor(1.0f, 1.0f, 0.0f);
	CreateAllObjectBuffer(&obj);
}

void CoreRenderer::RenderObjects(coreObjectBuffer *objBuffer)
{
	ID3D11DeviceContext* context = coreDevice->GetDeviceContext();

	//////////////////////////////////////////////////
	context->VSSetConstantBuffers(
		0,
		1,
		core_pConstantBuffer.GetAddressOf()
	);

	static float the_time = 0.0f;
	the_time += 3.14 / 180;
	if (the_time >= 3.14 * 2)
		the_time = 0.0f;

	ResetWorld();
	//RotateWorld(0.0f, the_time, 0.0f);
	TranslateWorld(0.0f, 0.0f, 0.0f);
	//ScaleWorld(0.2, 0.2, 0.2);
	//TranslateWorld(cos(the_time), sin(the_time), 0.0f);

	context->UpdateSubresource(
		core_pConstantBuffer.Get(),
		0,
		nullptr,
		&core_constantBufferData,
		0,
		0
	);

	//////////////////////////////////////////////////
	context->VSSetConstantBuffers(
		1,
		1,
		core_pNormalConstantBuffer.GetAddressOf()
	);

	UpdateInverseTranspose();
	 
	context->UpdateSubresource(
		core_pNormalConstantBuffer.Get(),
		0,
		nullptr,
		&core_normalConstantBufferData,
		0,
		0
	);
	//////////////////////////////////////////////////

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = sizeof(VertexPositionNormalColor);
	UINT offset = 0;

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	context->IASetVertexBuffers(
		0,
		1,
		objBuffer->objectVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	if(objBuffer->withIndices) 
	{
		context->IASetIndexBuffer(
			objBuffer->objectIndexBuffer.Get(),
			DXGI_FORMAT_R16_UINT,
			0
		);

		context->DrawIndexed(
			objBuffer->indicesCount,
			0,
			0
		);
	}
	else
	{
		context->Draw(
			objBuffer->verticesCount,
			0
		);
	}
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

		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	CD3D11_BUFFER_DESC cbDesc2(
		sizeof(NormalConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc2,
		nullptr,
		core_pNormalConstantBuffer.GetAddressOf()
	);

	fclose(vShader);
	fclose(pShader);

	return hr;
}

HRESULT CoreRenderer::CreateObjectBuffer(Object *obj)
{
	HRESULT hr = S_OK;	
	
	ID3D11Device* device = coreDevice->GetDevice();
	
	coreObjectBuffer objBuffer;
	
	// Create vertex buffer:
	CD3D11_BUFFER_DESC vDesc(
		sizeof(*(obj->getVertices())) * obj->getVerticesCount(),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_IMMUTABLE
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = obj->getVertices();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&objBuffer.objectVertexBuffer
	);

	objBuffer.verticesCount = obj->getVerticesCount();

	//create index buffer
	CD3D11_BUFFER_DESC iDesc(
		sizeof(*(obj->getIndices())) * obj->getIndicesCount(),
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = obj->getIndices();
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&iDesc,
		&iData,
		&objBuffer.objectIndexBuffer
	);

	objBuffer.indicesCount = obj->getIndicesCount();
	objBuffer.withIndices  = true;

	vObjectBuffer.push_back(objBuffer);

	return hr;
}

HRESULT CoreRenderer::CreateAllObjectBuffer(Object *obj)
{
	HRESULT hr = S_OK;

	ID3D11Device* device = coreDevice->GetDevice();
	
	coreObjectBuffer allObjBuffer;

	// Create vertex buffer:
	CD3D11_BUFFER_DESC vDesc(
		sizeof(*(obj->getAllVertices())) * obj->getAllVerticesCount(),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_IMMUTABLE
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = obj->getAllVertices();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&allObjBuffer.objectVertexBuffer
	);

	allObjBuffer.verticesCount = obj->getAllVerticesCount();
	allObjBuffer.withIndices   = false;

	vObjectBuffer.push_back(allObjBuffer);
	
	return hr;
}

void CoreRenderer::CreateViewAndPerspective()
{
	// Use DirectXMath to create view and perspective matrices.

	XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
	XMVECTOR eye = XMVectorSet(0.0f, 5.0f, 5.0f, 0.f);
	XMVECTOR at  = XMVectorSet(0.0f, 0.0f, 0.0f, 0.f);

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
