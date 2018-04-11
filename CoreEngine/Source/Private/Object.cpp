#include "Object.h"
#include "CoreUtils.h"

#include <fstream>
#include <sstream>
#include <ppltasks.h>

using namespace std;

Object::Object(const char *filename) :
	verticesCount(0),
	indicesCount(0),
	isLoaded(false)
{
	LoadObjectFromFile(filename);
}


Object::~Object()
{
	delete[] objectVertices;
	delete[] objectIndices;

}

void Object::SetColor(float red, float green, float blue)
{
	for (size_t i = 0; i < verticesCount; i++)
	{
		objectVertices[i].color = XMFLOAT3(red, green, blue);
	}
}

void Object::SetNormal(float nx, float ny, float nz)
{
	for (size_t i = 0; i < verticesCount; i++)
	{
		objectVertices[i].normal = XMFLOAT3(nx, ny, nz);
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
				//objectVertices[verticesCount].color = XMFLOAT3(0.0f, 0.0f, 1.0f);

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
 

				//XMFLOAT3 edge1 = XMFLOAT3(
				//	objectVertices[index_v2].pos.x - objectVertices[index_v1].pos.x,
				//	objectVertices[index_v2].pos.y - objectVertices[index_v1].pos.y,
				//	objectVertices[index_v2].pos.z - objectVertices[index_v1].pos.z
				//); 
				//

				//XMFLOAT3 edge2 = XMFLOAT3(
				//	objectVertices[index_v3].pos.x - objectVertices[index_v1].pos.x,
				//	objectVertices[index_v3].pos.y - objectVertices[index_v1].pos.y,
				//	objectVertices[index_v3].pos.z - objectVertices[index_v1].pos.z 
				//);

				XMVECTOR v_edge1 = XMVectorSet(
					objectVertices[index_v2].pos.x - objectVertices[index_v1].pos.x,
					objectVertices[index_v2].pos.y - objectVertices[index_v1].pos.y,
					objectVertices[index_v2].pos.z - objectVertices[index_v1].pos.z,
					1.0f
				);


				XMVECTOR v_edge2 = XMVectorSet(
					objectVertices[index_v3].pos.x - objectVertices[index_v1].pos.x,
					objectVertices[index_v3].pos.y - objectVertices[index_v1].pos.y,
					objectVertices[index_v3].pos.z - objectVertices[index_v1].pos.z,
					1.0f
				);

				XMFLOAT3 edge1, edge2;
				XMStoreFloat3(&edge1, v_edge1);
				XMStoreFloat3(&edge2, v_edge2);

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

				//XMFLOAT3 normal = XMFLOAT3(
				//	(edge1.y * edge2.z) - (edge1.z * edge2.y),
				//	(edge2.x * edge1.z) - (edge2.z * edge1.x),
				//	(edge1.x * edge2.y) - (edge1.y * edge2.x)
				//);

				XMVECTOR v_normal = XMVector3Cross(
					v_edge1,
					v_edge2
				);

				XMFLOAT3 normal;

				XMStoreFloat3(&normal, v_normal);
				LOG_STR_3("NORMAL: %f %f %f",
					normal.x,
					normal.y,
					normal.z
				)

				LOG_STR("====================================================")

				XMStoreFloat3(
					&objectVertices[index_v1].normal,
					XMVectorAdd( 
						XMLoadFloat3(&objectVertices[index_v1].normal),
						v_normal
					)
				);
				
				XMStoreFloat3(
					&objectVertices[index_v2].normal,
					XMVectorAdd(
						XMLoadFloat3(&objectVertices[index_v1].normal),
						v_normal
					)
				);

				XMStoreFloat3(
					&objectVertices[index_v3].normal,
					XMVectorAdd(
						XMLoadFloat3(&objectVertices[index_v1].normal),
						v_normal
					)
				);

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

