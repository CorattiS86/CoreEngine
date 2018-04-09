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
	if (!isLoaded)
		return;

	for (size_t i = 0; i < verticesCount; i++)
	{
		objectVertices[i].color = XMFLOAT3(red, green, blue);
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

	objectVertices	= new VertexPositionColor[verticesCount];
	objectIndices	= new unsigned short[indicesCount];

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
				//objectVertices[verticesCount].color = XMFLOAT3(0.0f, 0.0f, 0.0f);

				++verticesCount;

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

				objectIndices[indicesCount] = atoi(v1.c_str()) - 1;
				++indicesCount;
				objectIndices[indicesCount] = atoi(v2.c_str()) - 1;
				++indicesCount;
				objectIndices[indicesCount] = atoi(v3.c_str()) - 1;
				++indicesCount;

			}
		}

		inputFileStream.close();
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

