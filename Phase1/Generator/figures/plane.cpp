#include "plane.hpp"

#include <iostream>
#include <fstream>
#include <vector>

void generatePlane(const std::string &fileName, float length, int divisions)
{

	std::string path = "Output/" + fileName;
	std::ofstream outFile;
	outFile.open(path);

	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file: " << path << "\n";
		return;
	}

	float halfLength = length / 2.0f;
	float divisionSize = length / divisions;
	int verticesPerLine = divisions + 1;
	float **geral = new float *[(divisions + 1) * (divisions + 1)];
	int count = 0;

	// Write vertices
	for (int i = 0; i <= divisions; ++i)
	{
		for (int j = 0; j <= divisions; ++j)
		{
			float x = -halfLength + j * divisionSize;
			float z = -halfLength + i * divisionSize;
			float *ponto = new float[3];
			ponto[0] = x;
			ponto[1] = 0;
			ponto[2] = z;
			geral[count] = ponto;
			count++;
		}
	}

	// Write triangle definitions using the indices of the vertices
	for (int i = 0; i < divisions; ++i)
	{
		for (int j = 0; j < divisions; ++j)
		{
			int topLeft = i * verticesPerLine + j;
			int topRight = topLeft + 1;
			int bottomLeft = (i + 1) * verticesPerLine + j;
			int bottomRight = bottomLeft + 1;

			// First triangle (counter-clockwise)
			outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
			// Second triangle (counter-clockwise)
			outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << "\n";
		}
	}

	outFile.close();
}

void plane(char *file, float length, int divisions)
{
	generatePlane(file, length, divisions);
}
