#pragma once
#include <memory>
#include <DirectXMath.h>
#include "CoreDevice.h"
#include "CoreRenderable.h"
#include "CoreCamera.h"
#include "ShadowMap.h"
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
	void RenderScene(vector<CoreRenderable> renderables);

	void ScreenShot(CoreRenderable *mRenderable);

private:
	void RenderObjects(vector<CoreRenderable> renderables);
	void ComputeShadowMap(vector<CoreRenderable> renderables);

	shared_ptr<CoreDevice>		mCoreDevice;

	shared_ptr<CoreCamera>		mCamera;
	unique_ptr<ShadowMap>		mShadowMap;
};
