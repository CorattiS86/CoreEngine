#pragma once
#include <memory>
#include <DirectXMath.h>
#include "CoreDevice.h"
#include "CoreRenderable.h"
#include "CoreCamera.h"

using namespace std;
using namespace DirectX;

class CoreDrawer
{
public:
	CoreDrawer(shared_ptr<CoreDevice> coreDevice);
	~CoreDrawer();

	void SetRenderable(CoreRenderable *renderable);
	void SetCamera(shared_ptr<CoreCamera> camera);
	void Init();
	void Draw();
	void ScreenShot();

private:
	shared_ptr<CoreDevice>		mCoreDevice;

	shared_ptr<CoreCamera>		mCamera;
	unique_ptr<CoreRenderable>  mRenderable;
};