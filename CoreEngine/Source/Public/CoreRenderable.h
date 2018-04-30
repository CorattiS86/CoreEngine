#pragma once

#include <d3d11.h>
#include <memory>
#include "CoreDevice.h"

class CoreRenderable
{
	
	CoreRenderable(const CoreRenderable & r);
	CoreRenderable & operator= (const CoreRenderable & r);

public:
	CoreRenderable()
	{

	}

	~CoreRenderable()
	{
	
	}

	void setBackgroundColor(float *color) {
		mBackgroundColor[0] = color[0];
		mBackgroundColor[1] = color[1];
		mBackgroundColor[2] = color[2];
	}

	void setViewport(D3D11_VIEWPORT viewport) { 
		mViewport = viewport; 
	}

	void setRenderTargetViews(ComPtr<ID3D11RenderTargetView>* RTV) {
		mRTV.As(RTV);
	}
								
	void setDepthStencilView( ComPtr<ID3D11DepthStencilView>* DSV) {
		mDSV.As(DSV);
	}
							
	void setShaderResourceView( ComPtr<ID3D11ShaderResourceView>* SRV) {
		mSRV.As(SRV);
	}

	void setVertexBuffer( ComPtr<ID3D11Buffer>* vertexBuffer) {
		mVertexBuffer.As(vertexBuffer);
	}

	void setVerticesCount(unsigned int verticesCount) {
		mVerticesCount = verticesCount;
	}

	void setIndexBuffer( ComPtr<ID3D11Buffer>* indexBuffer) {
		mIndexBuffer.As(indexBuffer);
	}

	void setIndicesCount(unsigned int indicesCount) {
		mIndicesCount = indicesCount;
	}

	void setIsWithIndices(bool isWithIndices) {
		bIsWithIndices = isWithIndices;
	}
	
	void setInputLayout( ComPtr<ID3D11InputLayout>* inputLayout)	{
		mInputLayout.As(inputLayout);
	}
	
	void setStride(UINT stride) {
		mStride = stride;
	}

	void setOffset(UINT offset) {
		mOffset = offset;
	}

	void setConstantBuffer( ComPtr<ID3D11Buffer>* constantbuffer) {
		mConstantBuffer.As(constantbuffer);
	}

	void setSamplerState( ComPtr<ID3D11SamplerState>* samplerState) {
		mSamplerState.As(samplerState);
	}

	void setVertexShader( ComPtr<ID3D11VertexShader>* vertexShader) {
		mVertexShader.As(vertexShader);
	}

	void setPixelShader( ComPtr<ID3D11PixelShader>* pixelShader) {
		mPixelShader.As(pixelShader);
	}

	void Render(ID3D11DeviceContext	*context, ID3D11RenderTargetView* rtv) 
	{
		const float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

		//context->ClearRenderTargetView(
		//	mRTV.Get(),
		//	red
		//);

		//context->ClearDepthStencilView(
		//	mDSV.Get(),
		//	D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		//	1.0f,
		//	0
		//);

		//context->OMSetRenderTargets(
		//	1,
		//	mRTV.GetAddressOf(),
		//	mDSV.Get()
		//);

		//context->RSSetViewports(
		//	1,
		//	&mViewport
		//);

		//context->IASetInputLayout(mInputLayout.Get());

		//context->VSSetShader(
		//	mVertexShader.Get(),
		//	nullptr,
		//	0
		//);

		//context->PSSetShader(
		//	mPixelShader.Get(),
		//	nullptr,
		//	0
		//);

		//context->PSSetSamplers(
		//	0,
		//	1,
		//	mSamplerState.GetAddressOf()
		//);

		//context->IASetPrimitiveTopology(
		//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		//);

		//context->IASetVertexBuffers(
		//	0,
		//	1,
		//	mVertexBuffer.GetAddressOf(),
		//	&mStride,
		//	&mOffset
		//);

		//if (bIsWithIndices)
		//{
		//	context->IASetIndexBuffer(
		//		mIndexBuffer.Get(),
		//		DXGI_FORMAT_R16_UINT,
		//		0
		//	);

		//	context->DrawIndexed(
		//		mIndicesCount,
		//		0,
		//		0
		//	);
		//}
		//else
		//{
		//	context->Draw(
		//		mVerticesCount,
		//		0
		//	);
		//}

		
 	}

private:
	
	float								mBackgroundColor[3];
	D3D11_VIEWPORT						mViewport;
	ComPtr<ID3D11RenderTargetView>		mRTV;
	ComPtr<ID3D11DepthStencilView>		mDSV;
	ComPtr<ID3D11ShaderResourceView>	mSRV;
	ComPtr<ID3D11RasterizerState>		mRasterizerState;
	ComPtr<ID3D11Buffer>				mVertexBuffer;
	unsigned int						mVerticesCount;
	ComPtr<ID3D11Buffer>				mIndexBuffer;
	unsigned int						mIndicesCount;
	bool								bIsWithIndices;
	ComPtr<ID3D11InputLayout>			mInputLayout;
	UINT								mStride;
	UINT								mOffset;
	ComPtr<ID3D11Buffer>				mConstantBuffer;
	ComPtr<ID3D11VertexShader>			mVertexShader;
	ComPtr<ID3D11PixelShader>			mPixelShader;
	ComPtr<ID3D11SamplerState>			mSamplerState;

};



class CoreRenderableBuilder
{
public:

	CoreRenderableBuilder() { }

	CoreRenderableBuilder& buildBackgroundColor(float *color) {
		_BackgroundColor[0] = color[0];
		_BackgroundColor[1] = color[1];
		_BackgroundColor[2] = color[2];
		return *this;
	}

	CoreRenderableBuilder& buildViewport(D3D11_VIEWPORT viewport) {
		_Viewport = viewport;
		return *this;
	}

	CoreRenderableBuilder& buildRenderTargetViews(ComPtr<ID3D11RenderTargetView>* RTV) {
		_RTV.As(RTV);
		return *this;
	}

	CoreRenderableBuilder& buildDepthStencilView(ComPtr<ID3D11DepthStencilView>* DSV) {
		_DSV.As(DSV);
		return *this;
	}

	CoreRenderableBuilder& buildShaderResourceView(ComPtr<ID3D11ShaderResourceView>* SRV) {
		_SRV.As(SRV);
		return *this;
	}

	CoreRenderableBuilder& buildVertexBuffer(ComPtr<ID3D11Buffer>* vertexBuffer) {
		_VertexBuffer.As(vertexBuffer);
		return *this;
	}

	CoreRenderableBuilder& buildVerticesCount(unsigned int verticesCount) {
		_VerticesCount = verticesCount;
		return *this;
	}

	CoreRenderableBuilder& buildIndexBuffer(ComPtr<ID3D11Buffer>* indexBuffer) {
		_IndexBuffer.As(indexBuffer);
		return *this;
	}

	CoreRenderableBuilder& buildIndicesCount(unsigned int indicesCount) {
		_IndicesCount = indicesCount;
		return *this;
	}

	CoreRenderableBuilder& buildIsWithIndices(bool isWithIndices) {
		_IsWithIndices = isWithIndices;
		return *this;
	}

	CoreRenderableBuilder& buildInputLayout(ComPtr<ID3D11InputLayout>* inputLayout) {
		_InputLayout.As(inputLayout);
		return *this;
	}

	CoreRenderableBuilder& buildStride(UINT stride) {
		_Stride = stride;
		return *this;
	}

	CoreRenderableBuilder& buildOffset(UINT offset) {
		_Offset = offset;
		return *this;
	}

	CoreRenderableBuilder& buildConstantBuffer(ComPtr<ID3D11Buffer>* constantbuffer) {
		_ConstantBuffer.As(constantbuffer);
		return *this;
	}

	CoreRenderableBuilder& buildSamplerState(ComPtr<ID3D11SamplerState>* samplerState) {
		_SamplerState.As(samplerState);
		return *this;
	}

	CoreRenderableBuilder& buildVertexShader(ComPtr<ID3D11VertexShader>* vertexShader) {
		_VertexShader.As(vertexShader);
		return *this;
	}

	CoreRenderableBuilder& buildPixelShader(ComPtr<ID3D11PixelShader>* pixelShader) {
		_PixelShader.As(pixelShader);
		return *this;
	}

	CoreRenderable* buildRenderable() {

		CoreRenderable *_Renderable = new CoreRenderable;

		_Renderable->setBackgroundColor(_BackgroundColor);
		_Renderable->setViewport(_Viewport);
		_Renderable->setRenderTargetViews(&_RTV);
		_Renderable->setDepthStencilView(&_DSV);
		_Renderable->setShaderResourceView(&_SRV);
		_Renderable->setVertexBuffer(&_VertexBuffer);
		_Renderable->setVerticesCount(_VerticesCount);
		_Renderable->setIndexBuffer(&_IndexBuffer);
		_Renderable->setIndicesCount(_IndicesCount);
		_Renderable->setIsWithIndices(_IsWithIndices);
		_Renderable->setInputLayout(&_InputLayout);
		_Renderable->setStride(_Stride);
		_Renderable->setOffset(_Offset);
		_Renderable->setConstantBuffer(&_ConstantBuffer);
		_Renderable->setSamplerState(&_SamplerState);
		_Renderable->setVertexShader(&_VertexShader);
		_Renderable->setPixelShader(&_PixelShader);

		return _Renderable;
	}


private:

	float								_BackgroundColor[3];
	D3D11_VIEWPORT						_Viewport;
	ComPtr<ID3D11RenderTargetView>		_RTV;
	ComPtr<ID3D11DepthStencilView>		_DSV;
	ComPtr<ID3D11ShaderResourceView>	_SRV;
	ComPtr<ID3D11Buffer>				_VertexBuffer;
	unsigned int						_VerticesCount;
	ComPtr<ID3D11Buffer>				_IndexBuffer;
	unsigned int						_IndicesCount;
	bool								_IsWithIndices;
	ComPtr<ID3D11InputLayout>			_InputLayout;
	UINT								_Stride;
	UINT								_Offset;
	ComPtr<ID3D11Buffer>				_ConstantBuffer;
	ComPtr<ID3D11VertexShader>			_VertexShader;
	ComPtr<ID3D11PixelShader>			_PixelShader;
	ComPtr<ID3D11SamplerState>			_SamplerState;

};
