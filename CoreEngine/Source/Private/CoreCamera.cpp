#include "CoreCamera.h"
#include <d3d11.h>
#include "CoreUtils.h"

CoreCamera::CoreCamera(shared_ptr<CoreDevice> coreDevice)
	: mCoreDevice(coreDevice)
	, mUpDirection (0.0f, 1.0f, 0.0f)
	, mEyePosition (0.0f, 1.0f, 5.0f)
	, mLookAt      (0.0f, 0.0f, 0.0f)
{
	Init();	
}

CoreCamera::~CoreCamera()
{
	
}

void CoreCamera::Init()
{
	ZeroMemory(&mViewProjection, sizeof(mViewProjection));

	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat3(&mEyePosition),
				XMLoadFloat3(&mLookAt),
				XMLoadFloat3(&mUpDirection)
			)
		)
	);

	XMStoreFloat4x4(
		&mViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixPerspectiveFovRH(
				XMConvertToRadians(70),
				(4.0f / 3.0f),
				0.01f,
				100.0f
			)
		)
	);

	ID3D11Device		 *device  = mCoreDevice->GetDevice();
	ID3D11DeviceContext	 *context = mCoreDevice->GetDeviceContext();

	
	CD3D11_BUFFER_DESC cbDesc(
		sizeof(mViewProjection),
		D3D11_BIND_CONSTANT_BUFFER
	);

	HR(device->CreateBuffer(
		&cbDesc,
		nullptr,
		mViewProjectionConstantBuffer.GetAddressOf()
	));

	context->UpdateSubresource(
		mViewProjectionConstantBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);

	context->VSSetConstantBuffers(
		1,
		1,
		mViewProjectionConstantBuffer.GetAddressOf()
	);

	XMFLOAT4 mEyePositionPadded(
		mEyePosition.x,
		mEyePosition.y,
		mEyePosition.z,
		0.0f
	);

	CD3D11_BUFFER_DESC cbDesc2(
		sizeof(mEyePositionPadded),
		D3D11_BIND_CONSTANT_BUFFER
	);

	HR(device->CreateBuffer(
		&cbDesc2,
		nullptr,
		mEyePositionConstantBuffer.GetAddressOf()
	));

	context->UpdateSubresource(
		mEyePositionConstantBuffer.Get(),
		0,
		nullptr,
		&mEyePositionPadded,
		0,
		0
	);

	context->PSSetConstantBuffers(
		0,
		1,
		mEyePositionConstantBuffer.GetAddressOf()
	);
}

void CoreCamera::setUpDirection(float x, float y, float z)
{
	mUpDirection = XMFLOAT3(x, y, z);

	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat3(&mEyePosition),
				XMLoadFloat3(&mLookAt),
				XMLoadFloat3(&mUpDirection)
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mViewProjectionConstantBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);

}

void CoreCamera::setEyePosition(float x, float y, float z)
{
	mEyePosition = XMFLOAT3(x, y, z);
	
	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat3(&mEyePosition),
				XMLoadFloat3(&mLookAt),
				XMLoadFloat3(&mUpDirection)
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mViewProjectionConstantBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);

	XMFLOAT4 mEyePositionPadded(
		mEyePosition.x,
		mEyePosition.y,
		mEyePosition.z,
		1.0f
	);

	context->UpdateSubresource(
		mEyePositionConstantBuffer.Get(),
		0,
		nullptr,
		&mEyePositionPadded,
		0,
		0
	);
	context->PSSetConstantBuffers(
		0,
		1,
		mEyePositionConstantBuffer.GetAddressOf()
	);

}

void CoreCamera::setLookAt(float x, float y, float z)
{
	mLookAt = XMFLOAT3(x, y, z);

	XMStoreFloat4x4(
		&mViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				XMLoadFloat3(&mEyePosition),
				XMLoadFloat3(&mLookAt),
				XMLoadFloat3(&mUpDirection)
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mViewProjectionConstantBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);
}



void CoreCamera::setPerspectiveProjection(float aspectRatio)
{
	XMStoreFloat4x4(
		&mViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixPerspectiveFovRH(
				XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mViewProjectionConstantBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);

}

void CoreCamera::setOrthographicProjection()
{
	XMStoreFloat4x4(
		&mViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixOrthographicRH(
				5.0f,
				5.0f,
				0.01f,
				100.0f
			)
		)
	);

	ID3D11DeviceContext *context = mCoreDevice->GetDeviceContext();

	context->UpdateSubresource(
		mViewProjectionConstantBuffer.Get(),
		0,
		nullptr,
		&mViewProjection,
		0,
		0
	);

}
