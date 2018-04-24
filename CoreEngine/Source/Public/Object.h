#pragma once
#include "CoreCommon.h"

class Object
{

public:
	Object();
	~Object();

	bool IsLoaded() { return isLoaded; }
	void SetColor(float red, float green, float blue);
	void SetNormal(float nx, float ny, float nz);

	VertexPositionNormalColor*	getVertices()		  { return objectVertices;	   }
	unsigned int				getVerticesCount()	  { return verticesCount;	   }
													  
	unsigned short*				getIndices()		  { return objectIndices;	   }
	unsigned int				getIndicesCount()	  { return indicesCount; 	   }
	
	VertexPositionNormalColor*	getAllVertices()	  { return allObjectVertices;  }
	unsigned int				getAllVerticesCount() { return allVerticesCount;   }

	VertexPosNorColTex*			getTexVertices()	  { return texObjectVertices; }
	unsigned int				getTexVerticesCount() { return texVerticesCount; }

	void LoadTexObjectFromFile(const char* filename);
	void LoadTexObjectWithIndicesFromFile(const char* filename);

private:

	VertexPositionNormalColor		*objectVertices;
	unsigned int		 			verticesCount;
	
	VertexPositionNormalColor		*allObjectVertices;
	unsigned int					allVerticesCount;
	
	VertexPosNorColTex				*texObjectVertices;
	unsigned int		 			texVerticesCount;

	// indices
	unsigned short					*objectIndices;
	unsigned int					indicesCount;

	bool							isLoaded;
	


};

