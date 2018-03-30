#pragma once

#include "CoreDevice.h"
#include <memory>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class CoreRenderer
{
public:
	CoreRenderer(shared_ptr<CoreDevice> coreDevice);
	~CoreRenderer();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	void Update();
	void Render();

private:
	HRESULT CreateShaders();
	HRESULT CreateTriangle();
	HRESULT CreateCube();
	void    CreateViewAndPerspective();

	//-----------------------------------------------------------------------------
	// Pointer to device resource manager
	//-----------------------------------------------------------------------------
	shared_ptr<CoreDevice> coreDevice;

	//-----------------------------------------------------------------------------
	// Variables for rendering the cube
	//-----------------------------------------------------------------------------
	typedef struct _constantBufferStruct {
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	} ConstantBufferStruct;

	// Assert that the constant buffer remains 16-byte aligned.
	static_assert((sizeof(ConstantBufferStruct) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	//-----------------------------------------------------------------------------
	// Per-vertex data
	//-----------------------------------------------------------------------------
	typedef struct VERTEX_POSITION_COLOR
	{
		XMFLOAT3 pos;
		XMFLOAT3 color;
	} VertexPositionColor;

	//-----------------------------------------------------------------------------
	// Per-vertex data (extended)
	//-----------------------------------------------------------------------------
	typedef struct VERTEX_POSITION_NORMAL_TANGENT
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
	} VertexPositionColorTangent;

	ConstantBufferStruct core_constantBufferData;
	unsigned int  core_indexCount;
	unsigned int  core_frameCount;

	//-----------------------------------------------------------------------------
	// Direct3D device resources
	//-----------------------------------------------------------------------------
	//ID3DXEffect* m_pEffect;
	ComPtr<ID3D11Buffer>            core_pTriangleVertexBuffer;
	
	ComPtr<ID3D11Buffer>            core_pVertexBuffer;
	ComPtr<ID3D11Buffer>            core_pIndexBuffer;
	ComPtr<ID3D11Buffer>            core_pConstantBuffer;

	ComPtr<ID3D11VertexShader>      core_pVertexShader;
	ComPtr<ID3D11PixelShader>       core_pPixelShader;
	ComPtr<ID3D11InputLayout>       core_pInputLayout;
	ComPtr<ID3D11InputLayout>       core_pInputLayoutExtended;
	
};