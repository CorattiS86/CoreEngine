#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include "CoreDevice.h"
#include "CoreCommon.h"

using namespace std;
using namespace Microsoft::WRL;

class ShadowMap
{
	ShadowMap(const ShadowMap& rhs);
	ShadowMap& operator=(const ShadowMap& rhs);

public:
	ShadowMap(
		shared_ptr<CoreDevice> coreDevice, 
		UINT width, 
		UINT height
	);

	~ShadowMap();

	ID3D11ShaderResourceView* DepthMapSRV();
	ID3D11ShaderResourceView* PrecomputedDepthMapSRV();
	void ResetCoordinates();
	void SetPosition(float x, float y, float z);
	void SetLookAt(float x, float y, float z);
	void SetUpDirection(float x, float y, float z);

	void SetPerspectiveProjection(float aspectRatio);
	void SetOrthographicProjection();

	void BindDsvAndSetNullRenderTarget();
	void Clear();
	void SetForScreenshot();
	void Screenshot();
	void LoadMapping();

	ID3D11Buffer*	GetMatrixPositionBuffer()	{ return mMatrixPositionBuffer.Get();	}
	ID3D11Buffer*	GetViewProjBuffer()			{ return mLightViewProjBuffer.Get(); }

private:

	UINT mWidth;
	UINT mHeight;
			
	shared_ptr<CoreDevice>				mCoreDevice;

	ComPtr<ID3D11ShaderResourceView>	mPrecomputedDepthMapSRV;
	ComPtr<ID3D11ShaderResourceView>	mDepthMapSRV;
	ComPtr<ID3D11DepthStencilView>		mDepthMapDSV;
	ComPtr<ID3D11Texture2D>				mDepthMapTexture;

	XMFLOAT4							mUpDirection;
	XMFLOAT4							mLightPosition;
	XMFLOAT4							mLookAt;

	XMFLOAT4X4							mMatrixPosition;
	ComPtr<ID3D11Buffer>				mMatrixPositionBuffer;

	ViewProjection						mViewProjection;
	ComPtr<ID3D11Buffer>				mLightViewProjBuffer;

	D3D11_VIEWPORT						mViewport;
};
