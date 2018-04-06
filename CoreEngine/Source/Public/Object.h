#pragma once
#include "CoreCommon.h"

class Object
{

public:
	Object();
	~Object();

	void LoadObjectFromFile(const char* filename);
	bool IsLoaded() { return bIsLoaded; }

	VertexPositionColor		ObjectVertices[1000];
	unsigned int			VerticesCount;
	unsigned short			ObjectIndices[1000];
	unsigned int			IndicesCount;

private:

	bool					bIsLoaded;

	const char*				fileName;

};

