#include "Object.h"
#include "CoreUtils.h"

#include <fstream>
#include <sstream>
#include <ppltasks.h>

using namespace std;

Object::Object(const char *filename) :
	VerticesCount(0),
	IndicesCount(0),
	bIsLoaded(false),
	fileName(nullptr)
{
	LoadObjectFromFile(filename);
}


Object::~Object()
{

}

void Object::LoadObjectFromFile(const char *filename)
{
	VerticesCount	= 0;
	IndicesCount	= 0;
	bIsLoaded		= false;

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

			ObjectVertices[VerticesCount].pos = XMFLOAT3(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));
			ObjectVertices[VerticesCount].color = XMFLOAT3(1.0f, 0.0f, 0.0f);

			VerticesCount++;

		}

		if (inputLine[0] == 'f' && inputLine[1] == ' ')
		{

			istringstream iss(inputLine);
			string face;
			string v1, v2, v3;
			string vt1, vt2, vt3;
			string vn1, vn2, vn3;

			getline(iss, face, ' ');
			getline(iss, v1, '/'); getline(iss, vt1, '/'); getline(iss, vn1, ' ');
			getline(iss, v2, '/'); getline(iss, vt2, '/'); getline(iss, vn2, ' ');
			getline(iss, v3, '/'); getline(iss, vt3, '/'); getline(iss, vn3, '\n');

			ObjectIndices[IndicesCount] = atoi(v1.c_str()) - 1;
			++IndicesCount;
			ObjectIndices[IndicesCount] = atoi(v2.c_str()) - 1;
			++IndicesCount;
			ObjectIndices[IndicesCount] = atoi(v3.c_str()) - 1;
			++IndicesCount;

		}
	}

	for (int j = 0; j< VerticesCount; j++)
	{
		LOG_STR_3("VERTEX POS X: %f Y: %f Z: %f ",
			ObjectVertices[j].pos.x,
			ObjectVertices[j].pos.y,
			ObjectVertices[j].pos.z
		)
	}
	LOG_STR_1("VERTEX COUNT: %d ", VerticesCount)

	for (int j = 0; j< IndicesCount; j += 3)
	{
		LOG_STR_3("FACE POS v1: %d v2: %d v3: %d ",
			ObjectIndices[j],
			ObjectIndices[j + 1],
			ObjectIndices[j + 2]
		)
	}
	LOG_STR_1("INDEX COUNT: %d ", IndicesCount)

	bIsLoaded = true;
}

