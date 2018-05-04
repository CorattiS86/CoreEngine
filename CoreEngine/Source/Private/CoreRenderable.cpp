#include "CoreRenderable.h"

#include <DirectXMath.h>
using namespace DirectX;


void CoreRenderable::ResetCoordinates()
{
	XMStoreFloat4x4(
		&mWorldViewProjection.world,
		XMMatrixIdentity()
	);
}

void CoreRenderable::TranslateCoordinates(float axisX, float axisY, float axisZ)
{
	XMMATRIX translation =
		XMMatrixTranslation(
			axisX,
			axisY,
			axisZ
		);

	XMStoreFloat4x4(
		&mWorldViewProjection.world,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&mWorldViewProjection.world),
				translation
			)
		)
	);
}

void CoreRenderable::RotateCoordinates(float roll, float pitch, float yaw)
{
	XMFLOAT3 InputAngles(roll, pitch, yaw);
	XMVECTOR VectorAngles = XMLoadFloat3(&InputAngles);

	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(VectorAngles);

	XMStoreFloat4x4(
		&mWorldViewProjection.world,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&mWorldViewProjection.world),
				rotation
			)
		)
	);
}

void CoreRenderable::ScaleCoordinates(float Sx, float Sy, float Sz)
{
	XMFLOAT3 InputScale(Sx, Sy, Sz);
	XMVECTOR VectorAngles = XMLoadFloat3(&InputScale);

	XMMATRIX scale = XMMatrixScalingFromVector(VectorAngles);

	XMStoreFloat4x4(
		&mWorldViewProjection.world,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&mWorldViewProjection.world),
				scale
			)
		)
	);
}

void CoreRenderable::SetEyePosition(XMVECTOR up, XMVECTOR eye, XMVECTOR at)
{
	XMStoreFloat4x4(
		&mWorldViewProjection.view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				eye,
				at,
				up
			)
		)
	);
}

void CoreRenderable::SetPerspectiveProjection(float aspectRatio)
{
	XMStoreFloat4x4(
		&mWorldViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixPerspectiveFovRH(
				XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);
}

void CoreRenderable::SetOrthographicProjection()
{
	XMStoreFloat4x4(
		&mWorldViewProjection.projection,
		XMMatrixTranspose(
			XMMatrixOrthographicRH(
				5.0f,
				5.0f,
				0.01f,
				100.0f
			)
		)
	);

}


