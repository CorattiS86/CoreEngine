#pragma once
#include <memory>
#include <DirectXMath.h>
#include "CoreDevice.h"
#include "CoreRenderable.h"

using namespace std;
using namespace DirectX;

class CoreDrawer
{
public:
	CoreDrawer(shared_ptr<CoreDevice> coreDevice);
	~CoreDrawer();

	void InsertRenderable(CoreRenderable *renderable);
	void Init();
	void Draw();
	void ScreenShot();

private:
	shared_ptr<CoreDevice>		mCoreDevice;

	unique_ptr<CoreRenderable>  mRenderable;
};