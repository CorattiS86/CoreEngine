#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class CoreCamera
{
public:
	CoreCamera();
	~CoreCamera();

	void setUp	(float x, float y, float z);
	void setEye	(float x, float y, float z);
	void setAt	(float x, float y, float z);

	XMFLOAT4X4* getView();
	XMFLOAT4X4* getPerspectiveProjection(float aspectRatio);
	XMFLOAT4X4* getOrthographicProjection();

private:

	XMVECTOR up;
	XMVECTOR eye;
	XMVECTOR at;

	
};

