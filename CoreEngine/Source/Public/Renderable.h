#pragma once

#include <d3d11.h>
#include <memory>
#include "CoreDevice.h"

class RenderableState
{
	
	RenderableState(const RenderableState & r);
	RenderableState & operator= (const RenderableState & r);

public:
	RenderableState()
	{

	}

	~RenderableState()
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

	void setRenderTargetViews(ID3D11RenderTargetView* RTV) {
		mRTV = RTV;
	}
								
	void setDepthStencilView(ID3D11DepthStencilView* DSV) {
		mDSV = DSV;
	}
							
	void setShaderResourceView(ID3D11ShaderResourceView* SRV) {
		mSRV = SRV;
	}

	void setVertexBuffer(ID3D11Buffer* vertexBuffer) {
		mVertexBuffer = vertexBuffer;
	}

	void setVerticesCount(unsigned int verticesCount) {
		mVerticesCount = verticesCount;
	}

	void setIndexBuffer(ID3D11Buffer* indexBuffer) {
		mIndexBuffer = indexBuffer;
	}

	void setIndicesCount(unsigned int indicesCount) {
		mIndicesCount = indicesCount;
	}

	void setIsWithIndices(bool isWithIndices) {
		bIsWithIndices = isWithIndices;
	}
	
	void setInputLayout(ID3D11InputLayout* inputLayout)	{
		mInputLayout = inputLayout;
	}
	
	void setStride(UINT stride) {
		mStride = stride;
	}

	void setOffset(UINT offset) {
		mOffset = offset;
	}

	void setConstantBuffer(ID3D11Buffer* constantbuffer) {
		mConstantBuffer = constantbuffer;
	}

	void setSamplerState(ID3D11SamplerState* samplerState) {
		mSamplerState = samplerState;
	}

	void setVertexShader(ID3D11VertexShader* vertexShader) {
		mVertexShader = vertexShader;
	}

	void setPixelShader(ID3D11PixelShader* pixelShader) {
		mPixelShader = pixelShader;
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



class RenderableStateBuilder
{
public:

	RenderableStateBuilder() { }

	RenderableStateBuilder& buildBackgroundColor(float *color) {
		_BackgroundColor[0] = color[0];
		_BackgroundColor[1] = color[1];
		_BackgroundColor[2] = color[2];
		return *this;
	}

	RenderableStateBuilder& buildViewport(D3D11_VIEWPORT* viewport) {
		_Viewport = viewport;
		return *this;
	}

	RenderableStateBuilder& buildRenderTargetViews(ID3D11RenderTargetView* RTV) {
		_RTV = RTV;
		return *this;
	}

	RenderableStateBuilder& buildDepthStencilView(ID3D11DepthStencilView* DSV) {
		_DSV = DSV;
		return *this;
	}

	RenderableStateBuilder& buildShaderResourceView(ID3D11ShaderResourceView* SRV) {
		_SRV = SRV;
		return *this;
	}

	RenderableStateBuilder& buildVertexBuffer(ID3D11Buffer* vertexBuffer) {
		_VertexBuffer = vertexBuffer;
		return *this;
	}

	RenderableStateBuilder& buildVerticesCount(unsigned int verticesCount) {
		_VerticesCount = verticesCount;
		return *this;
	}

	RenderableStateBuilder& buildIndexBuffer(ID3D11Buffer* indexBuffer) {
		_IndexBuffer = indexBuffer;
		return *this;
	}

	RenderableStateBuilder& buildIndicesCount(unsigned int indicesCount) {
		_IndicesCount = indicesCount;
		return *this;
	}

	RenderableStateBuilder& buildIsWithIndices(bool isWithIndices) {
		_IsWithIndices = isWithIndices;
		return *this;
	}

	RenderableStateBuilder& buildInputLayout(ID3D11InputLayout* inputLayout) {
		_InputLayout = inputLayout;
		return *this;
	}

	RenderableStateBuilder& buildStride(UINT stride) {
		_Stride = stride;
		return *this;
	}

	RenderableStateBuilder& buildOffset(UINT offset) {
		_Offset = offset;
		return *this;
	}

	RenderableStateBuilder& buildConstantBuffer(ID3D11Buffer* constantbuffer) {
		_ConstantBuffer = constantbuffer;
		return *this;
	}

	RenderableStateBuilder& buildSamplerState(ID3D11SamplerState* samplerState) {
		_SamplerState = samplerState;
		return *this;
	}

	RenderableStateBuilder& buildVertexShader(ID3D11VertexShader* vertexShader) {
		_VertexShader = vertexShader;
		return *this;
	}

	RenderableStateBuilder& buildPixelShader(ID3D11PixelShader* pixelShader) {
		_PixelShader = pixelShader;
		return *this;
	}

	RenderableState* buildRenderable() {

		RenderableState *_Renderable = new RenderableState;

		_Renderable->setBackgroundColor(_BackgroundColor);
		_Renderable->setViewport(_Viewport);
		_Renderable->setRenderTargetViews(_RTV.Get());
		_Renderable->setDepthStencilView(_DSV.Get());
		_Renderable->setShaderResourceView(_SRV.Get());
		_Renderable->setVertexBuffer(_VertexBuffer.Get());
		_Renderable->setVerticesCount(_VerticesCount);
		_Renderable->setIndexBuffer(_IndexBuffer.Get());
		_Renderable->setIndicesCount(_IndicesCount);
		_Renderable->setIsWithIndices(_IsWithIndices);
		_Renderable->setInputLayout(_InputLayout.Get());
		_Renderable->setStride(_Stride);
		_Renderable->setOffset(_Offset);
		_Renderable->setConstantBuffer(_ConstantBuffer.Get());
		_Renderable->setSamplerState(_SamplerState.Get());
		_Renderable->setVertexShader(_VertexShader.Get());
		_Renderable->setPixelShader(_PixelShader.Get());

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
