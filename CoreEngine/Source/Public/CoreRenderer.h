#pragma once

#include "CoreDevice.h"
#include "CoreCommon.h"
#include "Object.h"

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
	void TranslateWorld(float axisX, float axisY, float axisZ);
	void RotateWorld(float roll, float pitch, float yaw);
	void ScaleWorld(float Sx, float Sy, float Sz);
	void ResetWorld();
	void SetStates();
	void Render();
	void RenderObject(Object *obj);

private:
	
	HRESULT CreateShaders();

	HRESULT CreateObjectBuffer(Object *obj);

	void    CreateViewAndPerspective();

	Object *mObject;
	//-----------------------------------------------------------------------------
	// Pointer to device resource manager
	//-----------------------------------------------------------------------------
	shared_ptr<CoreDevice> coreDevice;

	
	ConstantBufferStruct core_constantBufferData;
	unsigned int  core_indexCount;
	unsigned int  core_frameCount;

	//-----------------------------------------------------------------------------
	// Direct3D device resources
	//-----------------------------------------------------------------------------
	//ID3DXEffect* m_pEffect;

	ComPtr<ID3D11Buffer>            core_pObjectVertexBuffer;
	ComPtr<ID3D11Buffer>            core_pObjectIndexBuffer;
	
	ComPtr<ID3D11Buffer>            core_pConstantBuffer;

	ComPtr<ID3D11VertexShader>      core_pVertexShader;
	ComPtr<ID3D11PixelShader>       core_pPixelShader;
	ComPtr<ID3D11InputLayout>       core_pInputLayout;
	ComPtr<ID3D11InputLayout>       core_pInputLayoutExtended;
	

	ComPtr<ID3D11RasterizerState>	core_pRasterStateWireframeMode;
	ComPtr<ID3D11RasterizerState>	core_pRasterStateFillMode;

};