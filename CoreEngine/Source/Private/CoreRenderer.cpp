#include "CoreRenderer.h"
#include "CoreUtils.h"
#include "DDSTextureLoader.h"
#include <ppltasks.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ShadowMap.h"
#include "ScreenGrab.h"

using namespace std;
using namespace DirectX;

CoreRenderer::CoreRenderer(shared_ptr<CoreDevice> coreDevice)
	: core_frameCount(0)
	, mCoreDevice(coreDevice)
{

}

CoreRenderer::~CoreRenderer()
{
}

void CoreRenderer::CreateDeviceDependentResources()
{
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

void CoreRenderer::CreateGraphicalResources()
{
	auto AssemblyObjectsTask = Concurrency::create_task(
		[this]()
	{
		AssemblyTexObject("Resources/MonkeySmooth.obj");
		
		//AssemblyTexObject("Resources/CubeUV.obj");
		
		//AssemblyTexObject("Resources/Plane.obj");
	}
	);

	auto LoadTextureTask = Concurrency::create_task(
		[this]()
	{
		ID3D11Device*        device	 = mCoreDevice->GetDevice();
		ID3D11DeviceContext* context = mCoreDevice->GetDeviceContext();

		CreateDDSTextureFromFile(
			device,
			context,
			L"Resources/MonkeyDiffuse.dds",
			(ID3D11Resource**)mTexture.GetAddressOf(),
			mSRV.GetAddressOf()
		);

		CreateDDSTextureFromFile(
			device,
			context,
			L"Resources/MonkeyDiffuse2.dds",
			(ID3D11Resource**)mTexture2.GetAddressOf(),
			mSRV2.GetAddressOf()
		);

	}
	);
}

void CoreRenderer::Update()
{
	core_frameCount++;
	
	if (core_frameCount >= MAXUINT)  core_frameCount = 0;
}

void CoreRenderer::AssemblyTexObject(const char * filename)
{
	Object obj;
	obj.LoadTexObjectFromFile(filename);
	CreateTexObjectBuffer(&obj);
}

void CoreRenderer::AssemblyTexObjectWithIndices(const char * filename)
{
	Object obj;
	obj.LoadTexObjectWithIndicesFromFile(filename);
	CreateTexObjectBufferWithIndices(&obj);
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
	XMStoreFloat4x4(
		&core_constantBufferData.world, 
		XMMatrixIdentity()
	);
}

void CoreRenderer::UpdateOtherConstantBuffer()
{

	XMStoreFloat4(
		&core_otherConstantBufferData.eye,
		eye
	);

	
}

void CoreRenderer::SetStates()
{
	ID3D11Device* device = mCoreDevice->GetDevice();
	ID3D11DeviceContext* context = mCoreDevice->GetDeviceContext();

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

}

void CoreRenderer::Render()
{
	ID3D11Device* device = mCoreDevice->GetDevice();
	ID3D11DeviceContext* context = mCoreDevice->GetDeviceContext();

	ID3D11RenderTargetView* renderTarget = mCoreDevice->GetRenderTargetView();
	ID3D11DepthStencilView* depthStencil = mCoreDevice->GetDepthStencilView();
	D3D11_VIEWPORT			viewport	 = mCoreDevice->GetViewport();

	// Clear the render target and the z-buffer.
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.0f };
	const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

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

	/*context->RSSetViewports(
		1,
		&viewport
	);*/

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

	context->PSSetSamplers(
		0,
		1,
		mSamplerState.GetAddressOf()
	);

	// set Textures
	context->PSSetShaderResources(
		0,
		1,
		mSRV.GetAddressOf()
	);

	context->PSSetShaderResources(
		1,
		1,
		mSRV2.GetAddressOf()
	);

	context->RSSetState(core_pRasterStateFillMode.Get());

	context->RSSetViewports(
		1,
		&viewport
	);

	for (auto& obj : vObjectBuffer)
	{
		SetObjectsToRender(&obj);
		RenderObjects(&obj);
	}

}



void CoreRenderer::SetObjectsToRender(coreObjectBuffer * objBuffer)
{
	ID3D11DeviceContext* context = mCoreDevice->GetDeviceContext();

	static float the_time = 0.0f;
	the_time += 3.14f / 180;
	if (the_time >= 3.14 * 2)
		the_time = 0.0f;

	ResetWorld();
	//RotateWorld(0.0f, 0.0f, 3.14f/2);
	//RotateWorld(-the_time, -the_time, -the_time);
	//TranslateWorld(0.0f, 0.0f, -abs( 50 * sin(the_time)));
	//TranslateWorld(cos(the_time), sin(the_time), 0.0f);

	context->UpdateSubresource(
		core_pConstantBuffer.Get(),
		0,
		nullptr,
		&core_constantBufferData,
		0,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		core_pConstantBuffer.GetAddressOf()
	);

	//////////////////////////////////////////////////
	UpdateOtherConstantBuffer();

	context->UpdateSubresource(
		core_pOtherConstantBuffer.Get(),
		0,
		nullptr,
		&core_otherConstantBufferData,
		0,
		0
	);

	context->PSSetConstantBuffers(
		0,
		1,
		core_pOtherConstantBuffer.GetAddressOf()
	);

	//////////////////////////////////////////////////

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = sizeof(VertexPosNorColTex);
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

	if (objBuffer->withIndices)
	{
		context->IASetIndexBuffer(
			objBuffer->objectIndexBuffer.Get(),
			DXGI_FORMAT_R16_UINT,
			0
		);
	}
}

void CoreRenderer::RenderObjects(coreObjectBuffer *objBuffer)
{
	ID3D11DeviceContext* context = mCoreDevice->GetDeviceContext();

	if(objBuffer->withIndices) 
	{
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

void CoreRenderer::ComputeShadowMap()
{
	ID3D11Device* device = mCoreDevice->GetDevice();

	ShadowMap shadowMap(device, 800, 600);

	struct BoundingSphere
	{
		BoundingSphere(float centerX, float centerY, float centerZ, float radius ) : Center(centerX, centerY, centerZ), Radius(radius) {}
		
		XMFLOAT3 Center;
		float Radius;
	};
	BoundingSphere boundingSphere(0.0f, 0.0f, 0.0f, 15.0f);



}



void CoreRenderer::ScreenShotCustom(UINT width, UINT height)
{
	ID3D11Device		*device = mCoreDevice->GetDevice();
	ID3D11DeviceContext	*context = mCoreDevice->GetDeviceContext();

	//================================================================
	//	RENDER TEXTURE
	//================================================================
	D3D11_TEXTURE2D_DESC renderTexDesc;
	ZeroMemory(&renderTexDesc, sizeof(renderTexDesc));
	renderTexDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTexDesc.Width					= width;
	renderTexDesc.Height				= height;
	renderTexDesc.BindFlags				= D3D11_BIND_RENDER_TARGET;
	renderTexDesc.CPUAccessFlags		= 0;
	renderTexDesc.MiscFlags				= 0;
	renderTexDesc.ArraySize				= 1;
	renderTexDesc.MipLevels				= 1;
	renderTexDesc.SampleDesc.Count		= 1;
	renderTexDesc.SampleDesc.Quality	= 0;
	renderTexDesc.Usage					= D3D11_USAGE_DEFAULT;

	ComPtr<ID3D11Texture2D> renderTexture;

	HR(device->CreateTexture2D(
		&renderTexDesc,
		0,
		renderTexture.GetAddressOf()
	));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format				= renderTexDesc.Format;
	rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice	= 0;

	ComPtr<ID3D11RenderTargetView> _RTV;

	HR(device->CreateRenderTargetView(
		renderTexture.Get(),
		&rtvDesc,
		_RTV.GetAddressOf()
	));
	//================================================================



	//================================================================
	//	DEPTH TEXTURE
	//================================================================
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&renderTexDesc, sizeof(renderTexDesc));
	depthTexDesc.Format					= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.Width					= width;
	depthTexDesc.Height					= height;
	depthTexDesc.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags			= 0;
	depthTexDesc.MiscFlags				= 0;
	depthTexDesc.ArraySize				= 1;
	depthTexDesc.MipLevels				= 1;
	depthTexDesc.SampleDesc.Count		= 1;
	depthTexDesc.SampleDesc.Quality		= 0;
	depthTexDesc.Usage					= D3D11_USAGE_DEFAULT;

	ComPtr<ID3D11Texture2D> depthTexture;

	HR(device->CreateTexture2D(
		&depthTexDesc,
		0,
		depthTexture.GetAddressOf()
	));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Flags = 0;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11DepthStencilView> _DSV;

	HR(device->CreateDepthStencilView(
		depthTexture.Get(),
		&dsvDesc,
		_DSV.GetAddressOf()
	));
	//================================================================

	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	context->ClearRenderTargetView(
		_RTV.Get(),
		teal
	);

	context->ClearDepthStencilView(
		_DSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	context->OMSetRenderTargets(
		1,
		_RTV.GetAddressOf(),
		_DSV.Get()
	);

	D3D11_VIEWPORT _viewport;
	ZeroMemory(&_viewport, sizeof(_viewport));
	_viewport.Height	= 200.0f;
	_viewport.Width		= 200.0f;
	_viewport.TopLeftX	= 0.0f;
	_viewport.TopLeftY	= 0.0f;
	_viewport.MinDepth	= 0.0f;
	_viewport.MaxDepth	= 1.0f;

	context->RSSetViewports(
		1,
		&_viewport
	);

	for (auto& obj : vObjectBuffer)
	{
		RenderObjects(&obj);
	}

	//================================================================



	//================================================================
	//	SAVE TEXTUREs
	//================================================================
	SaveDDSTextureToFile(
		context,
		renderTexture.Get(),
		L"Snapshots/RenderTexture.dds"
	);

	SaveDDSTextureToFile(
		context,
		depthTexture.Get(),
		L"Snapshots/DepthTexture.dds"
	);
	//================================================================
}

void CoreRenderer::ScreenShotBackBuffer()
{
	ID3D11DeviceContext	*context = mCoreDevice->GetDeviceContext();

	ID3D11Texture2D*	bbRTVTex = mCoreDevice->GetBackBufferRTV_Texture();
	ID3D11Texture2D*	bbDSVTex = mCoreDevice->GetBackBufferDSV_Texture();

	//================================================================
	//	SAVE TEXTUREs
	//================================================================
	SaveDDSTextureToFile(
		context,
		bbRTVTex,
		L"Snapshots/RenderTexture.dds"
	);

	SaveDDSTextureToFile(
		context,
		bbDSVTex,
		L"Snapshots/DepthTexture.dds"
	);
	//================================================================
}

HRESULT CoreRenderer::CreateShaders()
{
	HRESULT hr = S_OK;

	// Use the Direct3D device to load resources into graphics memory.
	ID3D11Device* device = mCoreDevice->GetDevice();

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
		core_pVertexShader.GetAddressOf()
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

		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes,
		bytesRead,
		core_pInputLayout.GetAddressOf()
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
		sizeof(WorldViewProjection),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc,
		nullptr,
		core_pConstantBuffer.GetAddressOf()
	);

	CD3D11_BUFFER_DESC cbDesc2(
		sizeof(OtherConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc2,
		nullptr,
		core_pOtherConstantBuffer.GetAddressOf()
	);

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

	device->CreateSamplerState(
		&samplerDesc,
		mSamplerState.GetAddressOf()
	);

	fclose(vShader);
	fclose(pShader);

	return hr;
}


HRESULT CoreRenderer::CreateTexObjectBuffer(Object * obj)
{
	HRESULT hr = S_OK;

	ID3D11Device* device = mCoreDevice->GetDevice();

	coreObjectBuffer TexObjBuffer;

	// Create vertex buffer:
	CD3D11_BUFFER_DESC vDesc(
		sizeof(*(obj->getTexVertices())) * obj->getTexVerticesCount(),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_IMMUTABLE
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = obj->getTexVertices();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&TexObjBuffer.objectVertexBuffer
	);

	TexObjBuffer.verticesCount = obj->getTexVerticesCount();
	TexObjBuffer.withIndices = false;

	vObjectBuffer.push_back(TexObjBuffer);

	return hr;
}

HRESULT CoreRenderer::CreateTexObjectBufferWithIndices(Object * obj)
{
	HRESULT hr = S_OK;

	ID3D11Device* device = mCoreDevice->GetDevice();

	coreObjectBuffer TexObjBuffer;

	// Create vertex buffer:
	CD3D11_BUFFER_DESC vDesc(
		sizeof(*(obj->getTexVertices())) * obj->getTexVerticesCount(),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_IMMUTABLE
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = obj->getTexVertices();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&TexObjBuffer.objectVertexBuffer
	);

	TexObjBuffer.verticesCount = obj->getTexVerticesCount();

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
		&TexObjBuffer.objectIndexBuffer
	);

	TexObjBuffer.indicesCount = obj->getIndicesCount();
	TexObjBuffer.withIndices = true;

	// insert object into the vector
	vObjectBuffer.push_back(TexObjBuffer);

	return hr;
}

void CoreRenderer::CreateViewAndPerspective()
{
	// Use DirectXMath to create view and perspective matrices.
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

	float aspectRatio = mCoreDevice->GetAspectRatio();

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
