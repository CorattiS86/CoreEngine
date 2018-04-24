#include "Object.h"
#include "CoreUtils.h"

#include <fstream>
#include <sstream>
#include <ppltasks.h>

using namespace std;

Object::Object() :
	verticesCount(0),
	indicesCount(0),
	allVerticesCount(0),
	texVerticesCount(0),
	isLoaded(false),
	objectVertices(nullptr),
	allObjectVertices(nullptr),
	objectIndices(nullptr),
	texObjectVertices(nullptr)
{
	
}


Object::~Object()
{
	delete[] objectVertices;
	delete[] objectIndices;
	delete[] allObjectVertices;
	delete[] texObjectVertices;
}

void Object::SetColor(float red, float green, float blue)
{
	for (size_t i = 0; i < verticesCount; i++)
	{
		objectVertices[i].color    = XMFLOAT3(red, green, blue);
	}

	for (size_t i = 0; i < allVerticesCount; i++)
	{
		allObjectVertices[i].color = XMFLOAT3(red, green, blue);
	}
}

void Object::SetNormal(float nx, float ny, float nz)
{
	for (size_t i = 0; i < verticesCount; i++)
	{
		objectVertices[i].normal = XMFLOAT3(nx, ny, nz);
	}

	for (size_t i = 0; i < allVerticesCount; i++)
	{
		allObjectVertices[i].normal = XMFLOAT3(nx, ny, nz);
	}

	for (size_t i = 0; i < texVerticesCount; i++)
	{
		texObjectVertices[i].normal = XMFLOAT3(nx, ny, nz);
	}
}

void Object::LoadTexObjectFromFile(const char * filename)
{
	XMFLOAT3		*faceVertices		= nullptr;		
	unsigned int	 faceVerticesCount	= 0;

	XMFLOAT2		*faceTexcoords = nullptr;
	unsigned int	 faceTexcoordsCount = 0;

	XMFLOAT3		*faceNormals		= nullptr;		
	unsigned int	 faceNormalsCount	= 0;

	unsigned short  *faceIndices		= nullptr;
	unsigned int	 faceIndicesCount	= 0;

	isLoaded = false;

	{
		ifstream inputFileStream(filename);
		if (!inputFileStream)
			return;			// TODO: add throw exception

		string inputLine = " ";

		while (!inputFileStream.eof())
		{
			getline(inputFileStream, inputLine, '\n');

			if (inputLine[0] == 'v' && inputLine[1] == ' ')
			{
				++faceVerticesCount;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 't' && inputLine[2] == ' ')
			{
				++faceTexcoordsCount;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 'n' && inputLine[2] == ' ')
			{
				++faceNormalsCount;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{
				faceIndicesCount += 3;
			}
		}

		inputFileStream.close();
	}

	LOG_STR_1("V POSITIONs COUNT: %d ", faceVerticesCount)
	LOG_STR_1("V TEXCOORDs COUNT: %d ", faceTexcoordsCount)
	LOG_STR_1("V NORMALs   COUNT: %d ", faceNormalsCount)
	LOG_STR_1("V INDICES   COUNT: %d ", faceIndicesCount)

	//allocates memory for vertex structure
	texVerticesCount  = faceIndicesCount;
	texObjectVertices = new VertexPosNorColTex[texVerticesCount];

	// allocates memory for ausiliary data
	faceVertices	= new XMFLOAT3[faceVerticesCount];
	faceTexcoords	= new XMFLOAT2[faceTexcoordsCount];
	faceNormals		= new XMFLOAT3[faceNormalsCount];
	faceIndices		= new unsigned short[faceIndicesCount];

	faceVerticesCount = 0;
	faceTexcoordsCount	= 0;
	faceNormalsCount = 0;
	faceIndicesCount = 0;

	{
		ifstream inputFileStream(filename);
		if (!inputFileStream)
			return;			// TODO: add throw exception

		string inputLine = " ";

		while (!inputFileStream.eof())
		{
			getline(inputFileStream, inputLine, '\n');

			if (inputLine[0] == 'v' && inputLine[1] == ' ')
			{
				istringstream iss(inputLine);
				string vertex;
				string vx, vy, vz;

				getline(iss, vertex, ' ');
				getline(iss, vx, ' '); 
				getline(iss, vy, ' ');	
				getline(iss, vz, '\n');

				faceVertices[faceVerticesCount] = XMFLOAT3(
					atof(vx.c_str()), 
					atof(vy.c_str()), 
					atof(vz.c_str())
				);

				++faceVerticesCount;
				
				continue;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 't' && inputLine[2] == ' ')
			{
				istringstream iss(inputLine);
				string texcoord;
				string u, v;

				getline(iss, texcoord, ' ');
				getline(iss, u, ' '); 
				getline(iss, v, '\n');

				faceTexcoords[faceTexcoordsCount] = XMFLOAT2(
					atof(u.c_str()), 
					1 - atof(v.c_str())
				);

				++faceTexcoordsCount;

				continue;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 'n' && inputLine[2] == ' ')
			{
				istringstream iss(inputLine);
				string normal;
				string nx, ny, nz;

				getline(iss, normal, ' ');
				getline(iss, nx, ' '); 
				getline(iss, ny, ' '); 
				getline(iss, nz, '\n');

				faceNormals[faceNormalsCount] = XMFLOAT3(
					atof(nx.c_str()), 
					atof(ny.c_str()),
					atof(nz.c_str())
				);

				++faceNormalsCount;

				continue;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{
				istringstream iss(inputLine);
				string face;
				string v1, v2, v3;
				string t1, t2, t3;
				string n1, n2, n3;

				int	   index_v1, index_v2, index_v3;
				int	   index_t1, index_t2, index_t3;
				int	   index_n1, index_n2, index_n3;

				getline(iss, face, ' ');
				getline(iss, v1, '/'); getline(iss, t1, '/'); getline(iss, n1, ' ');
				getline(iss, v2, '/'); getline(iss, t2, '/'); getline(iss, n2, ' ');
				getline(iss, v3, '/'); getline(iss, t3, '/'); getline(iss, n3, '\n');

				LOG_STR("====================================================")
			
				index_v1 = atoi(v1.c_str()) - 1; 
				index_v2 = atoi(v2.c_str()) - 1; 
				index_v3 = atoi(v3.c_str()) - 1; 

				index_t1 = atoi(t1.c_str()) - 1; 
				index_t2 = atoi(t2.c_str()) - 1; 
				index_t3 = atoi(t3.c_str()) - 1; 

				index_n1 = atoi(n1.c_str()) - 1; 
				index_n2 = atoi(n2.c_str()) - 1; 
				index_n3 = atoi(n3.c_str()) - 1; 
				
				LOG_STR("FACE: ")
				LOG_STR_3("%d / %d / %d", index_v1, index_t1, index_n1)
				LOG_STR_3("%d / %d / %d", index_v2, index_t2, index_n2)
				LOG_STR_3("%d / %d / %d", index_v3, index_t3, index_n3)

				// create vertex structure, Position, Normals, Texture coordinates
				texObjectVertices[faceIndicesCount].pos			= faceVertices	[index_v1];
				texObjectVertices[faceIndicesCount].texcoord	= faceTexcoords	[index_t1];
				texObjectVertices[faceIndicesCount].normal		= faceNormals	[index_n1];
				faceIndicesCount++;

				texObjectVertices[faceIndicesCount].pos			= faceVertices	[index_v2];
				texObjectVertices[faceIndicesCount].texcoord	= faceTexcoords	[index_t2];
				texObjectVertices[faceIndicesCount].normal		= faceNormals	[index_n2];
				faceIndicesCount++;

				texObjectVertices[faceIndicesCount].pos			= faceVertices	[index_v3];
				texObjectVertices[faceIndicesCount].texcoord	= faceTexcoords	[index_t3];
				texObjectVertices[faceIndicesCount].normal		= faceNormals	[index_n3];
				faceIndicesCount++;

				continue;
			}

		}
		
		LOG_STR("====================================================")

		inputFileStream.close();
	}

	for (int j = 0; j< texVerticesCount; j++)
	{
		LOG_STR_3("V POS    X: %f Y: %f Z: %f ",
			texObjectVertices[j].pos.x,
			texObjectVertices[j].pos.y,
			texObjectVertices[j].pos.z
		)

		LOG_STR_2("V TEXCOORD U: %f V: %f ",
			texObjectVertices[j].texcoord.x,
			texObjectVertices[j].texcoord.y
		)

		LOG_STR_3("V NORMAL NX: %f NY: %f NZ: %f ",
			texObjectVertices[j].normal.x,
			texObjectVertices[j].normal.y,
			texObjectVertices[j].normal.z
		)
	}

	LOG_STR_1("VERTEX COUNT: %d ", texVerticesCount)

	isLoaded = true;
}

void Object::LoadTexObjectWithIndicesFromFile(const char * filename)
{
	XMFLOAT3		*faceVertices = nullptr;
	unsigned int	 faceVerticesCount = 0;

	XMFLOAT2		*faceTexcoords = nullptr;
	unsigned int	 faceTexcoordsCount = 0;

	XMFLOAT3		*faceNormals = nullptr;
	unsigned int	 faceNormalsCount = 0;

	unsigned short  *faceIndices = nullptr;
	unsigned int	 faceIndicesCount = 0;

	isLoaded = false;

	{
		ifstream inputFileStream(filename);
		if (!inputFileStream)
			return;			// TODO: add throw exception

		string inputLine = " ";

		while (!inputFileStream.eof())
		{
			getline(inputFileStream, inputLine, '\n');

			if (inputLine[0] == 'v' && inputLine[1] == ' ')
			{
				++faceVerticesCount;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 't' && inputLine[2] == ' ')
			{
				++faceTexcoordsCount;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 'n' && inputLine[2] == ' ')
			{
				++faceNormalsCount;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{
				faceIndicesCount += 3;
			}
		}

		inputFileStream.close();
	}

	LOG_STR_1("V POSITIONs COUNT: %d ", faceVerticesCount);
	LOG_STR_1("V TEXCOORDs COUNT: %d ", faceTexcoordsCount);
	LOG_STR_1("V NORMALs   COUNT: %d ", faceNormalsCount);
	LOG_STR_1("V INDICES   COUNT: %d ", faceIndicesCount); 

	//allocates memory for vertex structure
	texVerticesCount  = faceVerticesCount;
	texObjectVertices = new VertexPosNorColTex[faceVerticesCount];

	indicesCount      = faceIndicesCount;
	objectIndices	  = new unsigned short[faceIndicesCount];

	// allocates memory for ausiliary data
	faceVertices  = new XMFLOAT3[faceVerticesCount];
	faceTexcoords = new XMFLOAT2[faceTexcoordsCount];
	faceNormals	  = new XMFLOAT3[faceNormalsCount];
	faceIndices   = new unsigned short[faceIndicesCount];

	faceVerticesCount	= 0;
	faceTexcoordsCount	= 0;
	faceNormalsCount	= 0;
	faceIndicesCount	= 0;

	SetNormal(0.0f, 0.0f, 0.0f);

	{
		ifstream inputFileStream(filename);
		if (!inputFileStream)
			return;			// TODO: add throw exception

		string inputLine = " ";

		while (!inputFileStream.eof())
		{
			getline(inputFileStream, inputLine, '\n');

			if (inputLine[0] == 'v' && inputLine[1] == ' ')
			{
				istringstream iss(inputLine);
				string vertex;
				string vx, vy, vz;

				getline(iss, vertex, ' ');
				getline(iss, vx, ' ');
				getline(iss, vy, ' ');
				getline(iss, vz, '\n');

				faceVertices[faceVerticesCount] = XMFLOAT3(
					atof(vx.c_str()),
					atof(vy.c_str()),
					atof(vz.c_str())
				);

				++faceVerticesCount;

				continue;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 't' && inputLine[2] == ' ')
			{
				istringstream iss(inputLine);
				string texcoord;
				string u, v;

				getline(iss, texcoord, ' ');
				getline(iss, u, ' ');
				getline(iss, v, '\n');

				faceTexcoords[faceTexcoordsCount] = XMFLOAT2(
					atof(u.c_str()),
					1 - atof(v.c_str())
				);

				++faceTexcoordsCount;

				continue;
			}

			if (inputLine[0] == 'v' && inputLine[1] == 'n' && inputLine[2] == ' ')
			{
				istringstream iss(inputLine);
				string normal;
				string nx, ny, nz;

				getline(iss, normal, ' ');
				getline(iss, nx, ' ');
				getline(iss, ny, ' ');
				getline(iss, nz, '\n');

				faceNormals[faceNormalsCount] = XMFLOAT3(
					atof(nx.c_str()),
					atof(ny.c_str()),
					atof(nz.c_str())
				);

				++faceNormalsCount;

				continue;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{
				istringstream iss(inputLine);
				string face;
				string v1, v2, v3;
				string t1, t2, t3;
				string n1, n2, n3;
				
				int	   index_v1, index_v2, index_v3;
				int	   index_t1, index_t2, index_t3;
				int	   index_n1, index_n2, index_n3;

				getline(iss, face, ' ');
				getline(iss, v1, '/'); getline(iss, t1, '/'); getline(iss, n1, ' ');
				getline(iss, v2, '/'); getline(iss, t2, '/'); getline(iss, n2, ' ');
				getline(iss, v3, '/'); getline(iss, t3, '/'); getline(iss, n3, '\n');

				LOG_STR("====================================================");

				index_v1 = atoi(v1.c_str()) - 1;
				index_v2 = atoi(v2.c_str()) - 1;
				index_v3 = atoi(v3.c_str()) - 1;

				index_t1 = atoi(t1.c_str()) - 1;
				index_t2 = atoi(t2.c_str()) - 1;
				index_t3 = atoi(t3.c_str()) - 1;

				index_n1 = atoi(n1.c_str()) - 1;
				index_n2 = atoi(n2.c_str()) - 1;
				index_n3 = atoi(n3.c_str()) - 1;

				LOG_STR("FACE: ")
				LOG_STR_3("%d / %d / %d", index_v1, index_t1, index_n1);
				LOG_STR_3("%d / %d / %d", index_v2, index_t2, index_n2);
				LOG_STR_3("%d / %d / %d", index_v3, index_t3, index_n3);

				// create vertex structure, Position, Normals, Texture coordinates
				texObjectVertices[index_v1].pos		 =  faceVertices [index_v1];
				texObjectVertices[index_v1].texcoord =  faceTexcoords[index_t1];
				texObjectVertices[index_v1].normal.x += faceNormals  [index_n1].x;
				texObjectVertices[index_v1].normal.y += faceNormals  [index_n1].y;
				texObjectVertices[index_v1].normal.z += faceNormals  [index_n1].z;
				
				objectIndices[faceIndicesCount] = index_v1;
				++faceIndicesCount;

				texObjectVertices[index_v2].pos		 =  faceVertices [index_v2];
				texObjectVertices[index_v2].texcoord =  faceTexcoords[index_t2];
				texObjectVertices[index_v2].normal.x += faceNormals  [index_n2].x;
				texObjectVertices[index_v2].normal.y += faceNormals  [index_n2].y;
				texObjectVertices[index_v2].normal.z += faceNormals  [index_n2].z;

				objectIndices[faceIndicesCount] = index_v2;
				++faceIndicesCount;

				texObjectVertices[index_v3].pos		 =  faceVertices [index_v3];
				texObjectVertices[index_v3].texcoord =  faceTexcoords[index_t3];
				texObjectVertices[index_v3].normal.x += faceNormals  [index_n3].x;
				texObjectVertices[index_v3].normal.y += faceNormals  [index_n3].y;
				texObjectVertices[index_v3].normal.z += faceNormals  [index_n3].z;

				objectIndices[faceIndicesCount] = index_v3;
				++faceIndicesCount;

				continue;
			}

		}

		LOG_STR("====================================================");

		inputFileStream.close();
	}

	for (int j = 0; j< texVerticesCount; j++)
	{
		LOG_STR_3("V POS    X: %f Y: %f Z: %f ",
			texObjectVertices[j].pos.x,
			texObjectVertices[j].pos.y,
			texObjectVertices[j].pos.z
		);

		LOG_STR_2("V TEXCOORD U: %f V: %f ",
			texObjectVertices[j].texcoord.x,
			texObjectVertices[j].texcoord.y
		);

		LOG_STR_3("V NORMAL NX: %f NY: %f NZ: %f ",
			texObjectVertices[j].normal.x,
			texObjectVertices[j].normal.y,
			texObjectVertices[j].normal.z
		);
	}

	LOG_STR_1("VERTEX COUNT: %d ", texVerticesCount);

	isLoaded = true;
}

