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
														  
	VertexPosNorColTex*	getTexVertices()		{ return texObjectVertices; }
	unsigned int		getTexVerticesCount()	{ return texVerticesCount; }

	unsigned short*		getIndices()			{ return objectIndices;	   }
	unsigned int		getIndicesCount()		{ return indicesCount; 	   }

	void LoadTexObjectFromFile(const char* filename);
	void LoadTexObjectWithIndicesFromFile(const char* filename);

private:

	// vertices
	VertexPosNorColTex				*texObjectVertices;
	unsigned int		 			texVerticesCount;

	// indices
	unsigned short					*objectIndices;
	unsigned int					indicesCount;

	bool							isLoaded;
	


};

