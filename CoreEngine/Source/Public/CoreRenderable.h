#pragma once

#include <d3d11.h>
#include <memory>
#include "CoreDevice.h"
#include "CoreCommon.h"
#include <wrl.h>

using namespace Microsoft::WRL;

class CoreRenderableBuilder;


class CoreRenderable
{
	friend CoreRenderableBuilder;

	CoreRenderable() { } //class can not be directly instantiated from the client code
	CoreRenderable(const CoreRenderable & r); // cannot be copied
	CoreRenderable & operator= (const CoreRenderable & r); // cannot be copied
 
public:
	

	~CoreRenderable()
	{
	
	}

	void ResetCoordinates();
	void TranslateCoordinates(float axisX, float axisY, float axisZ);
	void RotateCoordinates(float roll, float pitch, float yaw);
	void ScaleCoordinates(float Sx, float Sy, float Sz);
	void SetEyePosition(XMVECTOR up, XMVECTOR eye, XMVECTOR at);
	void SetPerspectiveProjection(float aspectRatio);
	void SetOrthographicProjection();

	//================================================================
	// GETTERs
	//================================================================
	
	float* getBackgroundColor() {
		return mBackgroundColor;
	}

	D3D11_VIEWPORT getViewport() {
		return mViewport;
	}

	ID3D11RenderTargetView* getRenderTargetViews() {
		return mRTV;
	}

	ID3D11DepthStencilView* getDepthStencilView() {
		return mDSV;
	}

	ID3D11ShaderResourceView* getShaderResourceView() {
		return mSRV;
	}

	ID3D11RasterizerState* getRasterizerState() {
		return mRasterizerState;
	}

	ID3D11Buffer* getVertexBuffer() {
		return mVertexBuffer;
	}

	unsigned int getVerticesCount() {
		return mVerticesCount;
	}

	ID3D11Buffer* getIndexBuffer() {
		return mIndexBuffer;
	}

	unsigned int getIndicesCount() {
		return mIndicesCount;
	}

	bool getIsWithIndices() {
		return mIsWithIndices;
	}

	ID3D11InputLayout* getInputLayout() {
		return mInputLayout;
	}

	UINT getStride() {
		return mStride;
	}

	UINT getOffset() {
		return mOffset;
	}

	D3D_PRIMITIVE_TOPOLOGY getPrimitiveTopology() {
		return mPrimitiveTopology;
	}

	ID3D11Buffer* getConstantBuffer() {
		return mConstantBuffer;
	}

	WorldViewProjection getWorldViewProjection() {
		return mWorldViewProjection;
	}

	ID3D11SamplerState* getSamplerState() {
		return mSamplerState;
	}

	ID3D11VertexShader* getVertexShader() {
		return mVertexShader;
	}

	ID3D11PixelShader* getPixelShader() {
		return mPixelShader;
	}
 
private:
	
	// make setter methods private make class immutable
	void setBackgroundColor(float *color ) {
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

	void setRasterizerState(ID3D11RasterizerState* rasterizerState) {
		mRasterizerState = rasterizerState;
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
		mIsWithIndices = isWithIndices;
	}

	void setInputLayout(ID3D11InputLayout* inputLayout) {
		mInputLayout = inputLayout;
	}

	void setStride(UINT stride) {
		mStride = stride;
	}

	void setOffset(UINT offset) {
		mOffset = offset;
	}

	void setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology) {
		mPrimitiveTopology = primitiveTopology;
	}

	void setConstantBuffer(ID3D11Buffer* constantbuffer) {
		mConstantBuffer = constantbuffer;
	}

	void setWorldViewProjection(WorldViewProjection worldViewProjection) {
		mWorldViewProjection = worldViewProjection;
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

	float								 mBackgroundColor[3];
	D3D11_VIEWPORT						 mViewport;
	ID3D11RenderTargetView				*mRTV;
	ID3D11DepthStencilView				*mDSV;
	ID3D11ShaderResourceView			*mSRV;
	ID3D11RasterizerState				*mRasterizerState;
	ID3D11Buffer						*mVertexBuffer;
	unsigned int						 mVerticesCount;
	ID3D11Buffer						*mIndexBuffer;
	unsigned int						 mIndicesCount;
	bool								 mIsWithIndices;
	ID3D11InputLayout					*mInputLayout;
	UINT								 mStride;
	UINT								 mOffset;
	D3D_PRIMITIVE_TOPOLOGY				 mPrimitiveTopology;
	ID3D11Buffer						*mConstantBuffer;
	WorldViewProjection					 mWorldViewProjection;
	ID3D11VertexShader					*mVertexShader;
	ID3D11PixelShader					*mPixelShader;
	ID3D11SamplerState					*mSamplerState;

};






/**
*  BUILDER CLASS FOR CORE_RENDERABLE 
*/
 

class CoreRenderableBuilder
{
public:

	CoreRenderableBuilder()  { }
	virtual ~CoreRenderableBuilder() { }

	CoreRenderableBuilder& buildBackgroundColor(float * color) {
		_BackgroundColor[0] = color[0];
		_BackgroundColor[1] = color[1];
		_BackgroundColor[2] = color[2];
		return *this;
	}

	CoreRenderableBuilder& buildViewport(D3D11_VIEWPORT viewport) {
		_Viewport = viewport;
		return *this;
	}

	CoreRenderableBuilder& buildRenderTargetViews(ID3D11RenderTargetView* RTV) {
		_RTV = RTV;
		return *this;
	}

	CoreRenderableBuilder& buildDepthStencilView(ID3D11DepthStencilView* DSV) {
		_DSV = DSV;
		return *this;
	}

	CoreRenderableBuilder& buildShaderResourceView(ID3D11ShaderResourceView* SRV) {
		_SRV = SRV;
		return *this;
	}

	CoreRenderableBuilder& buildVertexBuffer(ID3D11Buffer* vertexBuffer) {
		_VertexBuffer = vertexBuffer;
		return *this;
	}

	CoreRenderableBuilder& buildVerticesCount(unsigned int verticesCount) {
		_VerticesCount = verticesCount;
		return *this;
	}

	CoreRenderableBuilder& buildIndexBuffer(ID3D11Buffer* indexBuffer) {
		_IndexBuffer = indexBuffer;
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

	CoreRenderableBuilder& buildStride(UINT stride) {
		_Stride = stride;
		return *this;
	}

	CoreRenderableBuilder& buildOffset(UINT offset) {
		_Offset = offset;
		return *this;
	}

	CoreRenderableBuilder& buildPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology) {
		_PrimitiveTopology = primitiveTopology;
		return *this;
	}

	CoreRenderableBuilder& buildConstantBuffer(ID3D11Buffer* constantbuffer) {
		_ConstantBuffer = constantbuffer;
		return *this;
	}

	CoreRenderableBuilder& buildWorldViewProjection(WorldViewProjection worldViewProjection) {
		_WorldViewProjection = worldViewProjection;
	}

	CoreRenderableBuilder& buildSamplerState(ID3D11SamplerState* samplerState) {
		_SamplerState = samplerState;
		return *this;
	}

	CoreRenderableBuilder& buildVertexShader(ID3D11VertexShader* vertexShader) {
		_VertexShader = vertexShader;
		return *this;
	}

	CoreRenderableBuilder& buildInputLayout(ID3D11InputLayout* inputLayout) {
		_InputLayout = inputLayout;
		return *this;
	}

	CoreRenderableBuilder& buildPixelShader(ID3D11PixelShader* pixelShader) {
		_PixelShader = pixelShader;
		return *this;
	}

	CoreRenderable* buildRenderable() {

		CoreRenderable *_Renderable = new CoreRenderable;

		_Renderable->setBackgroundColor(_BackgroundColor);
		_Renderable->setViewport(_Viewport);
		_Renderable->setRenderTargetViews(_RTV.Get());
		_Renderable->setDepthStencilView(_DSV.Get());
		_Renderable->setShaderResourceView(_SRV.Get());
		_Renderable->setRasterizerState(_RasterizerState.Get());
		_Renderable->setVertexBuffer(_VertexBuffer.Get());
		_Renderable->setVerticesCount(_VerticesCount);
		_Renderable->setIndexBuffer(_IndexBuffer.Get());
		_Renderable->setIndicesCount(_IndicesCount);
		_Renderable->setIsWithIndices(_IsWithIndices);
		_Renderable->setInputLayout(_InputLayout.Get());
		_Renderable->setStride(_Stride);
		_Renderable->setOffset(_Offset);
		_Renderable->setPrimitiveTopology(_PrimitiveTopology);
		_Renderable->setConstantBuffer(_ConstantBuffer.Get());
		_Renderable->setWorldViewProjection(_WorldViewProjection);
		_Renderable->setSamplerState(_SamplerState.Get());
		_Renderable->setVertexShader(_VertexShader.Get());
		_Renderable->setPixelShader(_PixelShader.Get());

		return _Renderable;
	}

protected:

	float								_BackgroundColor[3];
	D3D11_VIEWPORT						_Viewport;
	ComPtr<ID3D11RenderTargetView>		_RTV;
	ComPtr<ID3D11DepthStencilView>		_DSV;
	ComPtr<ID3D11ShaderResourceView>	_SRV;
	ComPtr<ID3D11RasterizerState>		_RasterizerState;
	ComPtr<ID3D11Buffer>				_VertexBuffer;
	unsigned int						_VerticesCount;
	ComPtr<ID3D11Buffer>				_IndexBuffer;
	unsigned int						_IndicesCount;
	bool								_IsWithIndices;
	ComPtr<ID3D11InputLayout>			_InputLayout;
	UINT								_Stride;
	UINT								_Offset;
	D3D_PRIMITIVE_TOPOLOGY				_PrimitiveTopology;
	ComPtr<ID3D11Buffer>				_ConstantBuffer;
	WorldViewProjection					_WorldViewProjection;
	ComPtr<ID3D11VertexShader>			_VertexShader;
	ComPtr<ID3D11PixelShader>			_PixelShader;
	ComPtr<ID3D11SamplerState>			_SamplerState;

};
