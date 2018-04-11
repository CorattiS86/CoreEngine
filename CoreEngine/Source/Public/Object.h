#pragma once
#include "CoreCommon.h"

class Object
{

public:
	Object(const char *filename);
	~Object();

	bool IsLoaded() { return isLoaded; }
	void SetColor(float red, float green, float blue);
	void SetNormal(float nx, float ny, float nz);

	VertexPositionNormalColor*	getVertices()		{ return objectVertices;	}
	unsigned int				getVerticesCount()	{ return verticesCount;		}

	unsigned short*				getIndices()		{ return objectIndices;		}
	unsigned int				getIndicesCount()	{ return indicesCount;		}
	

private:

	void LoadObjectFromFile(const char* filename);
	
	VertexPositionNormalColor	*objectVertices;
	unsigned int		 		verticesCount;
	unsigned short				*objectIndices;
	unsigned int				indicesCount;
	
	bool						isLoaded;
	
};

