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
}

void Object::LoadObjectFromFile(const char *filename)
{
	verticesCount   = 0;
	indicesCount	= 0;
	isLoaded		= false;

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
				++verticesCount;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{
				indicesCount += 3;
			}
		}

		inputFileStream.close();
	}

	objectVertices	= new VertexPositionNormalColor[verticesCount];
	objectIndices	= new unsigned short[indicesCount];
	
	SetNormal(0.0f, 0.0f, 0.0f);

	verticesCount = 0;
	indicesCount = 0;
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
				string x, y, z;

				getline(iss, vertex, ' ');
				getline(iss, x, ' '); getline(iss, y, ' ');	getline(iss, z, '\n');

				objectVertices[verticesCount].pos = XMFLOAT3(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));

				++verticesCount;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{

				istringstream iss(inputLine);
				string face;
				string v1, v2, v3;
				string vt1, vt2, vt3;
				string vn1, vn2, vn3;

				int	   index_v1, index_v2, index_v3;

				getline(iss, face, ' ');
				getline(iss, v1, '/'); getline(iss, vt1, '/'); getline(iss, vn1, ' ');
				getline(iss, v2, '/'); getline(iss, vt2, '/'); getline(iss, vn2, ' ');
				getline(iss, v3, '/'); getline(iss, vt3, '/'); getline(iss, vn3, '\n');

				index_v1 = atoi(v1.c_str()) - 1; LOG_STR_1("INDEX 1: %d", index_v1) 
				index_v2 = atoi(v2.c_str()) - 1; LOG_STR_1("INDEX 2: %d", index_v2)
				index_v3 = atoi(v3.c_str()) - 1; LOG_STR_1("INDEX 3: %d", index_v3)

				objectIndices[indicesCount] = index_v1;
				++indicesCount;				  
				objectIndices[indicesCount] = index_v2;
				++indicesCount;				  
				objectIndices[indicesCount] = index_v3;
				++indicesCount;

				XMFLOAT3 edge1 = XMFLOAT3(
					objectVertices[index_v2].pos.x - objectVertices[index_v1].pos.x,
					objectVertices[index_v2].pos.y - objectVertices[index_v1].pos.y,
					objectVertices[index_v2].pos.z - objectVertices[index_v1].pos.z
				);


				XMFLOAT3 edge2 = XMFLOAT3(
					objectVertices[index_v3].pos.x - objectVertices[index_v1].pos.x,
					objectVertices[index_v3].pos.y - objectVertices[index_v1].pos.y,
					objectVertices[index_v3].pos.z - objectVertices[index_v1].pos.z
				);

				LOG_STR_3("EDGE_1: %f %f %f ",
					edge1.x,
					edge1.y,
					edge1.z
				)

				LOG_STR_3("EDGE_2: %f %f %f ",
					edge2.x,
					edge2.y,
					edge2.z
				)

				XMFLOAT3 normal;
				XMStoreFloat3(
					&normal,
					XMVector3Normalize(
						XMVector3Cross(
							XMLoadFloat3(&edge1),
							XMLoadFloat3(&edge2)
						)
					)
				);

				LOG_STR_3("NORMAL: %f %f %f",
					normal.x,
					normal.y,
					normal.z
				)

				objectVertices[index_v1].normal.x += normal.x;
				objectVertices[index_v1].normal.y += normal.y;
				objectVertices[index_v1].normal.z += normal.z;

				objectVertices[index_v2].normal.x += normal.x;
				objectVertices[index_v2].normal.y += normal.y;
				objectVertices[index_v2].normal.z += normal.z;

				objectVertices[index_v3].normal.x += normal.x;
				objectVertices[index_v3].normal.y += normal.y;
				objectVertices[index_v3].normal.z += normal.z;
			}
		}

		inputFileStream.close();
	}

	for (int j = 0; j < verticesCount; j++) {
		XMStoreFloat3(
			&objectVertices[j].normal,
			XMVector3Normalize(
				XMLoadFloat3(
					&objectVertices[j].normal
				)
			)
		);
	}

	for (int j = 0; j< verticesCount; j++)
	{
		LOG_STR_3("VERTEX POS X: %f Y: %f Z: %f ",
			objectVertices[j].pos.x,
			objectVertices[j].pos.y,
			objectVertices[j].pos.z
		)
	}
	LOG_STR_1("VERTEX COUNT: %d ", verticesCount)

	for (int j = 0; j< verticesCount; j++)
	{
		LOG_STR_3("NORMAL X: %f Y: %f Z: %f ",
			objectVertices[j].normal.x,
			objectVertices[j].normal.y,
			objectVertices[j].normal.z
		)
	}
	
	for (int j = 0; j< indicesCount; j += 3)
	{
		LOG_STR_3("FACE POS v1: %d v2: %d v3: %d ",
			objectIndices[j],
			objectIndices[j + 1],
			objectIndices[j + 2]
		)
	}
	LOG_STR_1("INDEX COUNT: %d ", indicesCount)

	isLoaded = true;
}

void Object::LoadAllObjectFromFile(const char *filename)
{
	verticesCount    = 0;
	indicesCount	 = 0;
	allVerticesCount = 0;
	isLoaded	     = false;

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
				++verticesCount;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{
				indicesCount	 += 3;
				allVerticesCount += 3;
			}
		}

		inputFileStream.close();
	}

	objectVertices		= new VertexPositionNormalColor[verticesCount];
	objectIndices		= new unsigned short[indicesCount];
	allObjectVertices	= new VertexPositionNormalColor[allVerticesCount];

	SetNormal(0.0f, 0.0f, 0.0f);

	verticesCount	 = 0;
	indicesCount	 = 0;
	allVerticesCount = 0;
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
				string x, y, z;

				getline(iss, vertex, ' ');
				getline(iss, x, ' '); getline(iss, y, ' ');	getline(iss, z, '\n');

				objectVertices[verticesCount].pos = XMFLOAT3(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));

				++verticesCount;
			}

			if (inputLine[0] == 'f' && inputLine[1] == ' ')
			{

				istringstream iss(inputLine);
				string face;
				string v1, v2, v3;
				string vt1, vt2, vt3;
				string vn1, vn2, vn3;

				int	   index_v1, index_v2, index_v3;

				getline(iss, face, ' ');
				getline(iss, v1, '/'); getline(iss, vt1, '/'); getline(iss, vn1, ' ');
				getline(iss, v2, '/'); getline(iss, vt2, '/'); getline(iss, vn2, ' ');
				getline(iss, v3, '/'); getline(iss, vt3, '/'); getline(iss, vn3, '\n');

				index_v1 = atoi(v1.c_str()) - 1; LOG_STR_1("INDEX 1: %d", index_v1)
				index_v2 = atoi(v2.c_str()) - 1; LOG_STR_1("INDEX 2: %d", index_v2)
				index_v3 = atoi(v3.c_str()) - 1; LOG_STR_1("INDEX 3: %d", index_v3)

				XMFLOAT3 edge1 = XMFLOAT3(
					objectVertices[index_v2].pos.x - objectVertices[index_v1].pos.x,
					objectVertices[index_v2].pos.y - objectVertices[index_v1].pos.y,
					objectVertices[index_v2].pos.z - objectVertices[index_v1].pos.z
				); 
				

				XMFLOAT3 edge2 = XMFLOAT3(
					objectVertices[index_v3].pos.x - objectVertices[index_v1].pos.x,
					objectVertices[index_v3].pos.y - objectVertices[index_v1].pos.y,
					objectVertices[index_v3].pos.z - objectVertices[index_v1].pos.z 
				); 

				LOG_STR_3("EDGE_1: %f %f %f ",
					edge1.x,
					edge1.y,
					edge1.z
				)

				LOG_STR_3("EDGE_2: %f %f %f ",
					edge2.x,
					edge2.y,
					edge2.z
				)

				XMFLOAT3 normal;	
				XMStoreFloat3(
					&normal,
					XMVector3Normalize(
						XMVector3Cross(
							XMLoadFloat3(&edge1),
							XMLoadFloat3(&edge2)
						)
					)
				);

				LOG_STR_3("NORMAL: %f %f %f",
					normal.x,
					normal.y,
					normal.z
				)
					
				objectIndices[indicesCount]	= index_v1;
				++indicesCount;

				XMFLOAT3	color;
				XMStoreFloat3(&color, XMVectorAbs(XMLoadFloat3(&normal)));

				allObjectVertices[allVerticesCount].pos		= objectVertices[index_v1].pos;
				allObjectVertices[allVerticesCount].normal	= normal;
				allObjectVertices[allVerticesCount].color   = color;
				++allVerticesCount;

				objectIndices[indicesCount]	= index_v2;
				++indicesCount;

				allObjectVertices[allVerticesCount].pos		= objectVertices[index_v2].pos;
				allObjectVertices[allVerticesCount].normal	= normal;
				allObjectVertices[allVerticesCount].color   = color;
				++allVerticesCount;

				objectIndices[indicesCount]	= index_v3;
				++indicesCount;

				allObjectVertices[allVerticesCount].pos		= objectVertices[index_v3].pos;
				allObjectVertices[allVerticesCount].normal	= normal;
				allObjectVertices[allVerticesCount].color   = color;
				++allVerticesCount;

				LOG_STR("====================================================")
			}
		}

		inputFileStream.close();
	}

	for (int j = 0; j< allVerticesCount; j++)
	{
		LOG_STR_3("V POS    X: %f Y: %f Z: %f ",
			allObjectVertices[j].pos.x,
			allObjectVertices[j].pos.y,
			allObjectVertices[j].pos.z
		)

		LOG_STR_3("V NORMAL X: %f Y: %f Z: %f ",
			allObjectVertices[j].normal.x,
			allObjectVertices[j].normal.y,
			allObjectVertices[j].normal.z
		)
	}

	LOG_STR_1("VERTEX COUNT: %d ", allVerticesCount)

	isLoaded = true;
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
					atof(v.c_str())
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

