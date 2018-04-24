#pragma once

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class ShadowMap
{
public:
	ShadowMap(
		ID3D11Device * device,
		UINT width,
		UINT height
	);

	~ShadowMap();

	ID3D11ShaderResourceView * getSRVDepthMap() { return mSRV.Get(); }
	void BindResources(ID3D11DeviceContext *context);

private:
	// to avoid copy of this class
	ShadowMap(ShadowMap& shadowMap);
	ShadowMap& operator=(const ShadowMap& shadowMap);

private:

	struct SHADOWMAP_CONSTANT_BUFFER
	{

	}ShadowMapConstantBuffer;

	UINT mWidth;
	UINT mHeight;
	
	ComPtr<ID3D11DepthStencilView>		mDSV;
	ComPtr<ID3D11ShaderResourceView>	mSRV;

	D3D11_VIEWPORT						mViewport;

	ComPtr<ID3D11SamplerState>			mSamplerState;
	ComPtr<ID3D11Buffer>				mConstantBuffer0;
	ComPtr<ID3D11VertexShader>			mVertexShader;
	ComPtr<ID3D11PixelShader>			mPixelShader;
	ComPtr<ID3D11InputLayout>			mInputLayout;
	

	bool bIsInitialized;
};

