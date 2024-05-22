#include "cone.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Point
{
	float x, y, z;
};

void generateCone(const std::string &fileName, float radius, float height, int slices, int stacks)
{
	std::string path = "../Output/" + fileName;
	std::ofstream outFile;
	outFile.open(path);
	std::vector<Point> points(stacks * slices * 2);

	if (!outFile.is_open())
	{
		std::cerr << "Error opening file: " << fileName << "\n";
		return;
	}

	Point apex = { 0, height, 0 };
	points[0] = apex;

	//Base
	for (int slice = 0; slice < slices; ++slice)
	{
		float theta = (2.0f * M_PI * slice) / slices;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		Point ponto;
		ponto.x = x;
		ponto.y = 0;
		ponto.z = z;
		points[slice + 1] = ponto;
	}

	//Sides
	for (int stack = 1; stack < stacks; ++stack)
	{ 
		float y = height * stack / stacks;
		float r = radius * (1 - (float)stack / stacks);
		for (int slice = 0; slice < slices; ++slice)
		{
			float theta = (float)slice / slices * 2 * M_PI;
			float x = r * cos(theta);
			float z = r * sin(theta);
			Point ponto;
			ponto.x = x;
			ponto.y = y;
			ponto.z = z;
			points[stack * slices + slice + 1] = ponto;
		}
	}


	Point origin = { 0, 0, 0 };
	points[stacks * slices + 1] = origin;


	int apexIndex = 0;
	int baseStartIndex = 1;
	int originIndex = stacks * slices + 1;

	//Sides triangles
	for (int stack = 0; stack < stacks; ++stack)
	{
		for (int slice = 0; slice < slices; ++slice)
		{
            int bottomRight = baseStartIndex + (stack * slices) + slice;
            int bottomLeft = baseStartIndex + (stack * slices) + ((slice + 1) % slices);
            int topLeft = baseStartIndex + ((stack + 1) * slices) + ((slice + 1) % slices);
            int topRight = baseStartIndex + ((stack + 1) * slices) + slice;

			if (stack == stacks - 1)
			{
				outFile << "t: " << points[apexIndex].x << "," << points[apexIndex].y << "," << points[apexIndex].z << " " << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
			}
			else
			{

				outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " " << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
				outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " " << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";

			}
		}
	}

	//Base triangles
	for (int slice = 1; slice < slices; ++slice)
	{
		outFile << "t: " << points[slice].x << "," << points[slice].y << "," << points[slice].z << " " << points[(slice + 1)].x << "," << points[(slice + 1)].y << "," << points[(slice + 1)].z << " " << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";

	}
	outFile << "t: " << points[slices].x << "," << points[slices].y << "," << points[slices].z << " " << points[1].x << "," << points[1].y << "," << points[1].z << " " << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";

	outFile.close();
}

void cone(char *file, float radius, float height, int slices, int stacks)
{
	generateCone(file, radius, height, slices, stacks);
}