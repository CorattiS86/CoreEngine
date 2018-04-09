#pragma once
#include "CoreCommon.h"

class Object
{

public:
	Object(const char *filename);
	~Object();

	bool IsLoaded() { return bIsLoaded; }

	VertexPositionColor		ObjectVertices[1000];
	unsigned int			VerticesCount;
	unsigned short			ObjectIndices[1000];
	unsigned int			IndicesCount;

private:

	void LoadObjectFromFile(const char* filename);

	bool					bIsLoaded;
	const char*				fileName;

};

