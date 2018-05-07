#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
#include "CoreCommon.h"
#include "CoreDevice.h"
#include <memory>

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

class CoreCamera
{
public:
	CoreCamera(shared_ptr<CoreDevice> coreDevice);
	~CoreCamera();

	void setUpDirection	(float x, float y, float z);
	void setEyePosition	(float x, float y, float z);
	void setLookAt   	(float x, float y, float z);

	void setPerspectiveProjection(float aspectRatio);
	void setOrthographicProjection();

	ID3D11Buffer*	getViewProjectionConstantBuffer()	{ return mViewProjectionConstantBuffer.Get();	}
	ID3D11Buffer*	getEyePositionConstantBuffer()		{ return mEyePositionConstantBuffer.Get();		}

	void Init();
private:

	//void Init();

private:
	shared_ptr<CoreDevice> mCoreDevice;

	XMFLOAT3 mUpDirection;
	XMFLOAT3 mEyePosition;
	XMFLOAT3 mLookAt;

	ViewProjection			mViewProjection;
	ComPtr<ID3D11Buffer>	mViewProjectionConstantBuffer;
	ComPtr<ID3D11Buffer>	mEyePositionConstantBuffer;
};

