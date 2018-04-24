#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>

#include "CoreDevice.h"
#include "Object.h"

using namespace std;
using namespace Microsoft::WRL;

class RenderableObject
{
public:
	RenderableObject(
		shared_ptr<CoreDevice> coreDevice,
		UINT width,
		UINT height
	);

	~RenderableObject();

	void CreateVertexBuffer(const char * filename);
	void SetShaders();
	void BindResources();
	void Render();

private:
	// to avoid copy of this class
	RenderableObject(RenderableObject& shadowMap);
	RenderableObject& operator=(const RenderableObject& shadowMap);

private:

	shared_ptr<CoreDevice> mCoreDevice;

	struct CONSTANT_BUFFER_DATA
	{

	}constantBufferData;

	UINT mWidth;
	UINT mHeight;

	ComPtr<ID3D11RenderTargetView>		mRTV;
	ComPtr<ID3D11DepthStencilView>		mDSV;
	ComPtr<ID3D11ShaderResourceView>	mSRV;

	D3D11_VIEWPORT						mViewport;
	
	ComPtr<ID3D11Buffer>				mVertexBuffer;
	unsigned int						mVerticesCount;
	ComPtr<ID3D11Buffer>				mIndexBuffer;
	unsigned int						mIndicesCount;
	bool								bIsWithIndices;
	UINT								mStride;
	UINT								mOffset;

	ComPtr<ID3D11SamplerState>			mSamplerState;
	ComPtr<ID3D11Buffer>				mConstantBuffer0;
	ComPtr<ID3D11InputLayout>			mInputLayout;
	ComPtr<ID3D11VertexShader>			mVertexShader;
	ComPtr<ID3D11PixelShader>			mPixelShader;
};