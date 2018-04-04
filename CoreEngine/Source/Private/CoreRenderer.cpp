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

	auto CreateLoaderTask = Concurrency::create_task(
		[this]()
	{
		LoadObject();
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
				translation, 
				XMLoadFloat4x4(&core_constantBufferData.world))
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
				rotation,
				XMLoadFloat4x4(&core_constantBufferData.world))
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
				scale,
				XMLoadFloat4x4(&core_constantBufferData.world))
		)
	);
}

void CoreRenderer::ResetWorld()
{
	XMStoreFloat4x4(&core_constantBufferData.world, XMMatrixIdentity());
}

void CoreRenderer::SetStates()
{
	ID3D11Device* device = coreDevice->GetDevice();
	ID3D11DeviceContext* context = coreDevice->GetDeviceContext();

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;

	device->CreateRasterizerState(&rsDesc, core_pRasterStateWireframeMode.GetAddressOf());

	context->RSSetState(core_pRasterStateWireframeMode.Get());
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
		0);

	// Set the render target.
	context->OMSetRenderTargets(
		1,
		&renderTarget,
		depthStencil
	);

	context->IASetInputLayout(core_pInputLayout.Get());

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

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
	RotateWorld(.0f, the_time, .0f);
	ScaleWorld(0.5, 0.5, 0.5);
	//TranslateWorld(cos(the_time), sin(the_time), 0.0f);

	context->UpdateSubresource(
		core_pConstantBuffer.Get(),
		0,
		nullptr,
		&core_constantBufferData,
		0,
		0
	);

	context->IASetVertexBuffers(
		0,
		1,
		core_pObjectVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		core_pObjectIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);
	
	context->DrawIndexed(
		IndicesCount,
		0,
		0
	);

	/*ResetWorld();
	ScaleWorld(0.5, 0.5, 1.0);
	TranslateWorld(-cos(the_time), -sin(the_time), 0.0f);


	context->UpdateSubresource(
		core_pConstantBuffer.Get(),
		0,
		nullptr,
		&core_constantBufferData,
		0,
		0
	);

	context->DrawIndexed(
		core_SquareIndicesCount,
		core_TriangleIndicesCount,
		core_TriangleVerticesCount
	);

	ResetWorld();
	ScaleWorld(0.5, 0.5, 1.0);
	ScaleWorld(0.5, 0.5, 1.0);
	TranslateWorld(cos(the_time) - 1.0 , 1.0 + sin(the_time), 0.0f);


	context->UpdateSubresource(
		core_pConstantBuffer.Get(),
		0,
		nullptr,
		&core_constantBufferData,
		0,
		0
	);
 
	context->DrawIndexed(
		core_CircleIndicesCount,
		core_TriangleIndicesCount + core_SquareIndicesCount,
		core_TriangleVerticesCount + core_SquareVerticesCount
	);*/

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

HRESULT CoreRenderer::LoadObject()
{
	HRESULT hr = S_OK;

	VerticesCount = 0;
	IndicesCount = 0;

	ifstream inputFileStream("CubeT.obj");
	if (!inputFileStream)
		return -1;

	string inputLine = " ";

	while (!inputFileStream.eof())
	{
		getline(inputFileStream, inputLine, '\n');

		if (inputLine[0] == 'v' && inputLine[1] == ' ')
		{
			 
			istringstream iss(inputLine);
			string vertex;
			string x, y, z;

			getline(iss, vertex, ' ');
			getline(iss, x, ' '); getline(iss, y, ' ');	getline(iss, z, '\n');

			ObjectVertices[VerticesCount].pos = XMFLOAT3(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));
			ObjectVertices[VerticesCount].color = XMFLOAT3(1.0f, 0.0f, 0.0f);
			
			VerticesCount++;

		}

		if (inputLine[0] == 'f' && inputLine[1] == ' ')
		{
			 
			istringstream iss(inputLine);
			string face;
			string v1, v2, v3;
			string vt1, vt2, vt3;
			string vn1, vn2, vn3;

			getline(iss, face, ' ');
			getline(iss, v1, '/'); getline(iss, vt1, '/'); getline(iss, vn1, ' ');
			getline(iss, v2, '/'); getline(iss, vt2, '/'); getline(iss, vn2, ' ');
			getline(iss, v3, '/'); getline(iss, vt3, '/'); getline(iss, vn3, '\n');

			ObjectIndices[IndicesCount] = atoi(v1.c_str()) - 1;
			++IndicesCount;
			ObjectIndices[IndicesCount] = atoi(v2.c_str()) - 1;
			++IndicesCount;
			ObjectIndices[IndicesCount] = atoi(v3.c_str()) - 1;
			++IndicesCount;

		}
	}

	for(int j=0; j< VerticesCount; j++)
	{
		LOG_STR_3("VERTEX POS X: %f Y: %f Z: %f ",
			ObjectVertices[j].pos.x,
			ObjectVertices[j].pos.y,
			ObjectVertices[j].pos.z
		)
	}
	LOG_STR_1("VERTEX COUNT: %d ", VerticesCount)

	for (int j = 0; j< IndicesCount; j+=3)
	{
		LOG_STR_3("FACE POS v1: %d v2: %d v3: %d ",
			ObjectIndices[j],
			ObjectIndices[j+1],
			ObjectIndices[j+2]
		)
	}
	LOG_STR_1("INDEX COUNT: %d ", IndicesCount)
	
	ID3D11Device* device = coreDevice->GetDevice();

	// Create vertex buffer:
	CD3D11_BUFFER_DESC vDesc(
		sizeof(ObjectVertices),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_IMMUTABLE
	);


	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = ObjectVertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&core_pObjectVertexBuffer
	);

	//create index buffer
	CD3D11_BUFFER_DESC iDesc(
		sizeof(ObjectIndices),
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = ObjectIndices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&iDesc,
		&iData,
		&core_pObjectIndexBuffer
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
