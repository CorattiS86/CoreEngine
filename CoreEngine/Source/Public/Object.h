#pragma once
#include "CoreCommon.h"

class Object
{

public:
	Object(const char *filename);
	~Object();

	bool IsLoaded() { return isLoaded; }
	void SetColor(float red, float green, float blue);

	VertexPositionColor*	getVertices()		{ return objectVertices;	}
	unsigned int			getVerticesCount()	{ return verticesCount;		}

	unsigned short*			getIndices()		{ return objectIndices;		}
	unsigned int			getIndicesCount()	{ return indicesCount;		}
	

private:

	void LoadObjectFromFile(const char* filename);
	
	VertexPositionColor		*objectVertices;
	unsigned int		 	verticesCount;
	unsigned short			*objectIndices;
	unsigned int			indicesCount;
	
	bool					isLoaded;
	
};

