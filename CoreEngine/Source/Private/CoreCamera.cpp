#include "CoreCamera.h"

CoreCamera::CoreCamera()
	: up (XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	, eye(XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f))
	, at (XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
{
}

CoreCamera::~CoreCamera()
{
	
}

void CoreCamera::setUp(float x, float y, float z)
{
	up = XMVectorSet(x, y, z, 0.0f);
}

void CoreCamera::setEye(float x, float y, float z)
{
	eye = XMVectorSet(x, y, z, 0.0f);
}

void CoreCamera::setAt(float x, float y, float z)
{
	at = XMVectorSet(x, y, z, 0.0f);
}

XMFLOAT4X4* CoreCamera::getView()
{
	XMFLOAT4X4 *view = new XMFLOAT4X4();

	XMStoreFloat4x4(
		view,
		XMMatrixTranspose(
			XMMatrixLookAtRH(
				eye,
				at,
				up
			)
		)
	);

	return view;
}

XMFLOAT4X4 * CoreCamera::getPerspectiveProjection(float aspectRatio)
{
	XMFLOAT4X4 *projection = new XMFLOAT4X4();

	XMStoreFloat4x4(
		projection,
		XMMatrixTranspose(
			XMMatrixPerspectiveFovRH(
				XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);

	return projection;
}

XMFLOAT4X4 * CoreCamera::getOrthographicProjection()
{
	XMFLOAT4X4 *projection = new XMFLOAT4X4();

	XMStoreFloat4x4(
		projection,
		XMMatrixTranspose(
			XMMatrixOrthographicRH(
				5.0f,
				5.0f,
				0.01f,
				100.0f
			)
		)
	);

	return projection;
}
