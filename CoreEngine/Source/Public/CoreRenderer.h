#pragma once
#include <memory>
#include <DirectXMath.h>
#include "CoreDevice.h"
#include "CoreRenderable.h"
#include "CoreCamera.h"
#include <vector>

using namespace std;
using namespace DirectX;

class CoreRenderer
{
public:
	CoreRenderer(shared_ptr<CoreDevice> coreDevice);
	~CoreRenderer();

	void SetCamera(shared_ptr<CoreCamera> camera);
	void Render(CoreRenderable *mRenderable);
	void RenderAll(vector<CoreRenderable> renderables);

	void ScreenShot(CoreRenderable *mRenderable);

private:
	shared_ptr<CoreDevice>		mCoreDevice;

	shared_ptr<CoreCamera>		mCamera;
	
};
