#include "CoreRenderable.h"

#include <DirectXMath.h>
using namespace DirectX;


void CoreRenderable::ResetCoordinates()
{
	XMStoreFloat4x4(
		&mWorldCoordinatesMatrix,
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
		&mWorldCoordinatesMatrix,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&mWorldCoordinatesMatrix),
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
		&mWorldCoordinatesMatrix,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&mWorldCoordinatesMatrix),
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
		&mWorldCoordinatesMatrix,
		XMMatrixTranspose(
			XMMatrixMultiply(
				XMLoadFloat4x4(&mWorldCoordinatesMatrix),
				scale
			)
		)
	);
}

