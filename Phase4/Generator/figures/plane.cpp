#include "plane.hpp"

#include <iostream>
#include <fstream>
#include <vector>

struct Point
{
	float x, y, z;
};

void generatePlane(const std::string &fileName, float length, int divisions)
{

	std::string path = "../Output/" + fileName;
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
	std::vector<Point> points;

	// Vertices of the plane
	for (int i = 0; i <= divisions; ++i)
	{
		for (int j = 0; j <= divisions; ++j)
		{
			float x = -halfLength + j * divisionSize;
			float z = -halfLength + i * divisionSize;
			Point ponto;
			ponto.x = x;
			ponto.y = 0;
			ponto.z = z;
			points.push_back(ponto);
		}
	}

	int size = points.size();
	//Normals of each vertex
	for (int i = 0; i < size; ++i)
	{
		outFile << "n: " << 0 << "," << 1 << "," << 0 << "\n";
	}

	//Triangles of the plane
	for (int i = 0; i < divisions; ++i)
	{
		for (int j = 0; j < divisions; ++j)
		{
			int topLeft = i * verticesPerLine + j;
			int topRight = topLeft + 1;
			int bottomLeft = (i + 1) * verticesPerLine + j;
			int bottomRight = bottomLeft + 1;

			outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " " << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
			outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " " << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";

		}
	}

	outFile.close();
}

void plane(char *file, float length, int divisions)
{
	generatePlane(file, length, divisions);
}
