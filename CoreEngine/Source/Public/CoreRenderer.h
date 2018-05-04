#pragma once
#include <memory>
#include <DirectXMath.h>
#include "CoreDevice.h"
#include "CoreRenderable.h"
#include "CoreCamera.h"

using namespace std;
using namespace DirectX;

class CoreRenderer
{
public:
	CoreRenderer(shared_ptr<CoreDevice> coreDevice);
	~CoreRenderer();

	void SetRenderable(CoreRenderable *renderable);
	void SetCamera(shared_ptr<CoreCamera> camera);
	void Init();
	void Render();
	void ScreenShot();

private:
	shared_ptr<CoreDevice>		mCoreDevice;

	shared_ptr<CoreCamera>		mCamera;
	unique_ptr<CoreRenderable>  mRenderable;
};