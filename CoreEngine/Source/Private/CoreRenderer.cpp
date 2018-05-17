#include "CoreRenderer.h"
#include "CoreUtils.h"
#include "ScreenGrab.h"
#include "ShadowMap.h"
#include <d3d11.h>
#include "DDSTextureLoader.h"


CoreRenderer::CoreRenderer(shared_ptr<CoreDevice> coreDevice)
	: mCoreDevice(coreDevice)
{
	
}

CoreRenderer::~CoreRenderer()
{
}

void CoreRenderer::SetCamera(shared_ptr<CoreCamera> camera)
{
	mCamera = camera;
}


void CoreRenderer::Render(CoreRenderable *mRenderable)
{
	if ( mRenderable == nullptr)
		return;

	ID3D11Device			*device = mCoreDevice->GetDevice();
	ID3D11DeviceContext		*context = mCoreDevice->GetDeviceContext();

	//================================================================
	// SETTING RESOURCES
	//================================================================

	context->RSSetViewports(
		1,
		&mRenderable->getViewport()
	);

	// Set up the vertex shader stage.
	context->VSSetShader(
		mRenderable->getVertexShader(),
		nullptr,
		0
	);

	// Set up the pixel shader stage.
	context->PSSetShader(
		mRenderable->getPixelShader(),
		nullptr,
		0
	);

	ID3D11SamplerState* samplerStates[1] = { mRenderable->getSamplerState() };

	context->PSSetSamplers(
		0,
		1,
		samplerStates
	);

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = mRenderable->getStride();
	UINT offset = mRenderable->getOffset();

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	ID3D11Buffer* vertexBuffers[1] = { mRenderable->getVertexBuffer() };

	context->IASetVertexBuffers(
		0,
		1,
		vertexBuffers,
		&stride,
		&offset
	);

	context->IASetInputLayout(
		mRenderable->getInputLayout()
	);

	ID3D11ShaderResourceView* shaderResourceViews[1] = { mRenderable->getShaderResourceView() };

	context->PSSetShaderResources(
		0,
		1,
		shaderResourceViews
	);

	context->RSSetState(
		mRenderable->getRasterizerState()
	);

	if (mRenderable->getIsWithIndices())
	{
		context->IASetIndexBuffer(
			mRenderable->getIndexBuffer(),
			DXGI_FORMAT_R16_UINT,
			0
		);
	}

	//================================================================
	// CLEAR, UPDATE & DRAW
	//================================================================

	context->ClearRenderTargetView(
		mRenderable->getRenderTargetViews(),
		mRenderable->getBackgroundColor()
	);

	context->ClearDepthStencilView(
		mRenderable->getDepthStencilView(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	ID3D11RenderTargetView* renderTargets[1] = { mRenderable->getRenderTargetViews() };

	context->OMSetRenderTargets(
		1,
		renderTargets,
		mRenderable->getDepthStencilView()
	);


	//================================================================
	// UPDATE WORLD and VIEW-PROJECTION MATRICES
	//================================================================
	{

		/*static float the_time = 0.0f;
		the_time += 3.14f / 180;
		if (the_time >= 3.14 * 2)
			the_time = 0.0f;

		mRenderable->ResetCoordinates();
		mRenderable->RotateCoordinates(-the_time, -the_time, -the_time);*/


		context->UpdateSubresource(
			mRenderable->getWorldConstantBuffer(),
			0,
			nullptr,
			mRenderable->getWorldCoordinatesMatrix(),
			0,
			0
		);

		ID3D11Buffer* constantBuffers[1] = { mRenderable->getWorldConstantBuffer() };

		context->VSSetConstantBuffers(
			0,
			1,
			constantBuffers
		);
	}

	//================================================================
	// DRAWING OPERATIONS
	//================================================================
	if (mRenderable->getIsWithIndices())
	{
		context->DrawIndexed(
			mRenderable->getIndicesCount(),
			0,
			0
		);
	}
	else
	{
		context->Draw(
			mRenderable->getVerticesCount(),
			0
		);
	}
		

	
}

void CoreRenderer::RenderScene(vector<CoreRenderable> renderables)
{
	ComputeShadowMap(renderables);
	RenderObjects(renderables);
}

void CoreRenderer::ComputeShadowMap(vector<CoreRenderable> renderables)
{
	ID3D11Device			*device = mCoreDevice->GetDevice();
	ID3D11DeviceContext		*context = mCoreDevice->GetDeviceContext();

	unique_ptr<ShadowMap> shadowMap = unique_ptr<ShadowMap>(new ShadowMap(mCoreDevice, 800, 600));
	mShadowMap = std::move(shadowMap);

	//================================================================
	// SETTING RESOURCES
	//================================================================
	//mShadowMap->SetForScreenshot();
	mShadowMap->LoadMapping();
	
	mShadowMap->BindDsvAndSetNullRenderTarget();
	mShadowMap->Clear();
	mShadowMap->SetPosition(5.0f, 0.0f, 0.0f);
	mShadowMap->SetPerspectiveProjection(4.0f / 3.0f);

	for (auto renderable : renderables)
	{
		CoreRenderable	*mRenderable = &renderable;

		// Set up the vertex shader stage.
		context->VSSetShader(
			mRenderable->getVertexShader(),
			nullptr,
			0
		);

		// Set up the pixel shader stage.
		context->PSSetShader(
			mRenderable->getPixelShader(),
			nullptr,
			0
		);

		ID3D11SamplerState* samplerStates[1] = { mRenderable->getSamplerState() };

		context->PSSetSamplers(
			0,
			1,
			samplerStates
		);

		// Set up the IA stage by setting the input topology and layout.
		UINT stride = mRenderable->getStride();
		UINT offset = mRenderable->getOffset();

		context->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

		ID3D11Buffer* vertexBuffers[1] = { mRenderable->getVertexBuffer() };

		context->IASetVertexBuffers(
			0,
			1,
			vertexBuffers,
			&stride,
			&offset
		);

		context->IASetInputLayout(
			mRenderable->getInputLayout()
		);

		context->RSSetState(
			mRenderable->getRasterizerState()
		);

		if (mRenderable->getIsWithIndices())
		{
			context->IASetIndexBuffer(
				mRenderable->getIndexBuffer(),
				DXGI_FORMAT_R16_UINT,
				0
			);
		}

		//================================================================
		// UPDATE WORLD and VIEW-PROJECTION MATRICES
		//================================================================
		{	

			ID3D11Buffer* constantBuffers[2] = { 
				mShadowMap->GetMatrixPositionBuffer(),
				mShadowMap->GetViewProjBuffer()
			};

			context->VSSetConstantBuffers(
				0,
				2,
				constantBuffers
			);
		}

		//================================================================
		// DRAWING OPERATIONS
		//================================================================
		if (mRenderable->getIsWithIndices())
		{
			context->DrawIndexed(
				mRenderable->getIndicesCount(),
				0,
				0
			);
		}
		else
		{
			context->Draw(
				mRenderable->getVerticesCount(),
				0
			);
		}


		//mShadowMap->Screenshot();
	}
}

void CoreRenderer::RenderObjects(vector<CoreRenderable> renderables)
{
	ID3D11Device			*device = mCoreDevice->GetDevice();
	ID3D11DeviceContext		*context = mCoreDevice->GetDeviceContext();

	D3D11_VIEWPORT			 viewport = mCoreDevice->GetViewport();
	ID3D11RenderTargetView   *rtv = mCoreDevice->GetRenderTargetView();
	ID3D11DepthStencilView	 *dsv = mCoreDevice->GetDepthStencilView();

	float teal[]	= { 0.098f, 0.439f, 0.439f, 1.0f };
	float orange[]	= { 1.0f, 0.50f, 0.01f, 1.0f };
	float gray[]	= { 0.3f, 0.3f, 0.3f, 1.0f };

	//================================================================
	// SETTING RESOURCES
	//================================================================

	context->RSSetViewports(
		1,
		&viewport
	);

	context->ClearRenderTargetView(
		rtv,
		gray
	);

	context->ClearDepthStencilView(
		dsv,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	ID3D11RenderTargetView* renderTargets[1] = { rtv };

	context->OMSetRenderTargets(
		1,
		renderTargets,
		dsv
	);


	for (auto renderable : renderables)
	{
		CoreRenderable	*mRenderable = &renderable;

		// Set up the vertex shader stage.
		context->VSSetShader(
			mRenderable->getVertexShader(),
			nullptr,
			0
		);

		// Set up the pixel shader stage.
		context->PSSetShader(
			mRenderable->getPixelShader(),
			nullptr,
			0
		);

		ID3D11SamplerState* samplerStates[1] = { mRenderable->getSamplerState() };

		context->PSSetSamplers(
			0,
			1,
			samplerStates
		);

		// Set up the IA stage by setting the input topology and layout.
		UINT stride = mRenderable->getStride();
		UINT offset = mRenderable->getOffset();

		context->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);

		ID3D11Buffer* vertexBuffers[1] = { mRenderable->getVertexBuffer() };

		context->IASetVertexBuffers(
			0,
			1,
			vertexBuffers,
			&stride,
			&offset
		);

		context->IASetInputLayout(
			mRenderable->getInputLayout()
		);

		
		ID3D11ShaderResourceView* shaderResourceViews[2] = { 
			mRenderable->getShaderResourceView(),
			//mShadowMap->DepthMapSRV() 
			mShadowMap->PrecomputedDepthMapSRV()
		};

		context->PSSetShaderResources(
			0,
			2,
			shaderResourceViews
		);

		context->RSSetState(
			mRenderable->getRasterizerState()
		);

		if (mRenderable->getIsWithIndices())
		{
			context->IASetIndexBuffer(
				mRenderable->getIndexBuffer(),
				DXGI_FORMAT_R16_UINT,
				0
			);
		}

		//================================================================
		// UPDATE WORLD and VIEW-PROJECTION MATRICES
		//================================================================
		{
			context->UpdateSubresource(
				mRenderable->getWorldConstantBuffer(),
				0,
				nullptr,
				mRenderable->getWorldCoordinatesMatrix(),
				0,
				0
			);
			
			ID3D11Buffer* constantBuffers[2] = { 
				mRenderable->getWorldConstantBuffer(),
				mCamera->getViewProjectionConstantBuffer()
			};

			context->VSSetConstantBuffers(
				0,
				2,
				constantBuffers
			);
		}

		
		//================================================================
		// DRAWING OPERATIONS
		//================================================================
		if (mRenderable->getIsWithIndices())
		{
			context->DrawIndexed(
				mRenderable->getIndicesCount(),
				0,
				0
			);
		}
		else
		{
			context->Draw(
				mRenderable->getVerticesCount(),
				0
			);
		}

	}

}

void CoreRenderer::ScreenShot(CoreRenderable *mRenderable)
{
	ID3D11Device			*device = mCoreDevice->GetDevice();
	ID3D11DeviceContext		*context = mCoreDevice->GetDeviceContext();

	D3D11_TEXTURE2D_DESC backbufferRTVDesc = mCoreDevice->GetBackBufferRTV_TextureDesc();
	D3D11_TEXTURE2D_DESC backbufferDSVDesc = mCoreDevice->GetBackBufferDSV_TextureDesc();

	//================================================================
	//	RENDER TEXTURE
	//================================================================
	D3D11_TEXTURE2D_DESC renderTexDesc;
	ZeroMemory(&renderTexDesc, sizeof(renderTexDesc));
	renderTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTexDesc.Width = backbufferRTVDesc.Width;
	renderTexDesc.Height = backbufferRTVDesc.Height;
	renderTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	renderTexDesc.CPUAccessFlags = 0;
	renderTexDesc.MiscFlags = 0;
	renderTexDesc.ArraySize = 1;
	renderTexDesc.MipLevels = 1;
	renderTexDesc.SampleDesc.Count = 1;
	renderTexDesc.SampleDesc.Quality = 0;
	renderTexDesc.Usage = D3D11_USAGE_DEFAULT;

	ComPtr<ID3D11Texture2D> renderTexture;

	HR(device->CreateTexture2D(
		&renderTexDesc,
		0,
		renderTexture.GetAddressOf()
	));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = renderTexDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11RenderTargetView> _RTV;

	HR(device->CreateRenderTargetView(
		renderTexture.Get(),
		&rtvDesc,
		_RTV.GetAddressOf()
	));
	//================================================================



	//================================================================
	//	DEPTH TEXTURE
	//================================================================
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&renderTexDesc, sizeof(renderTexDesc));
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.Width = backbufferDSVDesc.Width;
	depthTexDesc.Height = backbufferDSVDesc.Height;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;

	ComPtr<ID3D11Texture2D> depthTexture;

	HR(device->CreateTexture2D(
		&depthTexDesc,
		0,
		depthTexture.GetAddressOf()
	));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Flags = 0;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11DepthStencilView> _DSV;

	HR(device->CreateDepthStencilView(
		depthTexture.Get(),
		&dsvDesc,
		_DSV.GetAddressOf()
	));
	//================================================================

	context->ClearRenderTargetView(
		_RTV.Get(),
		mRenderable->getBackgroundColor()
	);

	context->ClearDepthStencilView(
		_DSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

	ID3D11RenderTargetView* renderTargets[1] = { _RTV.Get() };

	context->OMSetRenderTargets(
		1,
		renderTargets,
		_DSV.Get()
	);

	context->RSSetViewports(
		1,
		&mRenderable->getViewport()
	);

	context->IASetInputLayout(
		mRenderable->getInputLayout()
	);

	// Set up the vertex shader stage.
	context->VSSetShader(
		mRenderable->getVertexShader(),
		nullptr,
		0
	);

	// Set up the pixel shader stage.
	context->PSSetShader(
		mRenderable->getPixelShader(),
		nullptr,
		0
	);

	ID3D11SamplerState* samplerStates[1] = { mRenderable->getSamplerState() };

	context->PSSetSamplers(
		0,
		1,
		samplerStates
	);

	ID3D11ShaderResourceView* shaderResourceViews[1] = { mRenderable->getShaderResourceView() };

	context->PSSetShaderResources(
		0,
		1,
		shaderResourceViews
	);

	//context->RSSetState();

	static float the_time = 0.0f;
	the_time += 3.14f / 180;
	if (the_time >= 3.14 * 2)
		the_time = 0.0f;

	mRenderable->ResetCoordinates();
	mRenderable->RotateCoordinates(-the_time, -the_time, -the_time);


	context->UpdateSubresource(
		mRenderable->getWorldConstantBuffer(),
		0,
		nullptr,
		mRenderable->getWorldCoordinatesMatrix(),
		0,
		0
	);

	ID3D11Buffer* constantBuffers[1] = { mRenderable->getWorldConstantBuffer() };

	context->VSSetConstantBuffers(
		0,
		1,
		constantBuffers
	);


	//////////////////////////////////////////////////

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = mRenderable->getStride();
	UINT offset = mRenderable->getOffset();

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	ID3D11Buffer* vertexBuffers[1] = { mRenderable->getVertexBuffer() };

	context->IASetVertexBuffers(
		0,
		1,
		vertexBuffers,
		&stride,
		&offset
	);

	if (mRenderable->getIsWithIndices())
	{
		context->IASetIndexBuffer(
			mRenderable->getIndexBuffer(),
			DXGI_FORMAT_R16_UINT,
			0
		);

		context->DrawIndexed(
			mRenderable->getIndicesCount(),
			0,
			0
		);
	}
	else
	{
		context->Draw(
			mRenderable->getVerticesCount(),
			0
		);
	}

	//================================================================
	//	SAVE TEXTUREs
	//================================================================
	SaveDDSTextureToFile(
		context,
		renderTexture.Get(),
		L"Snapshots/RenderTexture.dds"
	);

	SaveDDSTextureToFile(
		context,
		depthTexture.Get(),
		L"Snapshots/DepthTexture.dds"
	);
	//================================================================

}
