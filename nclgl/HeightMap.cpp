#include "HeightMap.h"

HeightMap::HeightMap(std::string name, int map) {
	counter = 0;
	std::ifstream file(name.c_str(), ios::binary);
	if (!file) {
		return;

	}
	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1)*(RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];	unsigned char * data = new unsigned char[numVertices];
	file.read((char *)data, numVertices * sizeof(unsigned char));
	file.close();
	//magma
	if (map == 0) {
		for (int x = 0; x < RAW_WIDTH; ++x) {
			for (int z = 0; z < RAW_HEIGHT; ++z) {
				int offset = (x * RAW_WIDTH) + z;

				if (data[offset] * HEIGHTMAP_Y <= 200) {
					vertices[offset] = Vector3(
						x * HEIGHTMAP_X, data[offset] * HEIGHTMAP_Y, z * HEIGHTMAP_Z);
				}
				else {
					vertices[offset] = Vector3(
						x * HEIGHTMAP_X, 200, z * HEIGHTMAP_Z);
				}


				textureCoords[offset] = Vector2(
					x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);

			}

		}
	}
	//sea
	else if (map == 1) {
		for (int x = 0; x < RAW_WIDTH; ++x) {
			for (int z = 0; z < RAW_HEIGHT; ++z) {
				int offset = (x * RAW_WIDTH) + z;

				vertices[offset] = Vector3(
					x * HEIGHTMAP_X, 0, z * HEIGHTMAP_Z);
				
				textureCoords[offset] = Vector2(
					x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
			}

		}
	}
	
	//space
	else {
		for (int x = 0; x < RAW_WIDTH; ++x) {
			for (int z = 0; z < RAW_HEIGHT; ++z) {
				int offset = (x * RAW_WIDTH) + z;
				vertices[offset] = Vector3(
					x * HEIGHTMAP_X, -data[offset] * HEIGHTMAP_Y, z * HEIGHTMAP_Z);


				textureCoords[offset] = Vector2(
					x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);

			}

		}
	}
	


	delete data;

	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x) {
		for (int z = 0; z < RAW_HEIGHT - 1; ++z) {
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;

		}

	}
	GenerateNormals();
	GenerateTangents();
	BufferData();}void HeightMap::UpdateMap() {

	counter++;

	for (int x = 0; x < RAW_WIDTH; ++x) {

		for (int z = 0; z < RAW_HEIGHT; ++z) {
			int offset = (x * RAW_WIDTH) + z;

			vertices[offset] = Vector3(
				x * HEIGHTMAP_X,10*sin((x + counter)/20.0f) + 10*sin((z + counter) / 20.0f), z * HEIGHTMAP_Z);

		}
	}
	//GenerateNormals();
	//GenerateTangents();
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	glBindVertexArray(0);
	//BufferData();
}