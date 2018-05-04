#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "CoreCommon.h"

using namespace Microsoft::WRL;
using namespace DirectX;

class ShadowMap
{
public:
	ShadowMap(
		ID3D11Device * device,
		UINT width,
		UINT height
	);

	~ShadowMap();

	ID3D11ShaderResourceView*	getSRVDepthMap() { return mSRV.Get(); }
	void BindResources(ID3D11DeviceContext *context);
	void SetPointOfView(
		XMVECTOR up,
		XMVECTOR eye,
		XMVECTOR at
	);

	void Capture(ID3D11DeviceContext	*context);

private:
	// to avoid copy of this class
	ShadowMap(ShadowMap& shadowMap);
	ShadowMap& operator=(const ShadowMap& shadowMap);

private:
	
	ComPtr<ID3D11Texture2D>				mDepthMapTexture;

	ID3D11Device*						mDevice;
	ID3D11DeviceContext*				mContext;

	WorldViewProjection				mShadowMapConstantBuffer;

	UINT mWidth;
	UINT mHeight;
	
	ComPtr<ID3D11DepthStencilView>		mDSV;
	ComPtr<ID3D11ShaderResourceView>	mSRV;

	D3D11_VIEWPORT						mViewport;

	ComPtr<ID3D11SamplerState>			mSamplerState;
	ComPtr<ID3D11Buffer>				mConstantBuffer;
	ComPtr<ID3D11VertexShader>			mVertexShader;
	ComPtr<ID3D11PixelShader>			mPixelShader;
	ComPtr<ID3D11InputLayout>			mInputLayout;

	bool bIsInitialized;
};

