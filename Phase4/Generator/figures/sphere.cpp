#include "sphere.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct  Point
{
	float x, y, z;
};


void generateSphere(const std::string &fileName, float radius, int slices, int stacks)
{

	std::string path = "../Output/" + fileName;
	std::ofstream outFile;
	outFile.open(path);
	std::vector<Point> points ((stacks + 1) * (slices + 1));

	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file: " << path << "\n";
		return;
	}

	// Vertices of the sphere
	for (int stack = 0; stack <= stacks; ++stack)
	{
		float phi = M_PI / 2 - (float)stack / stacks * M_PI; 
		float y = radius * sin(phi);						
		float r = radius * cos(phi);

		for (int slice = 0; slice <= slices; ++slice)
		{
			Point ponto;
			float theta = (float)slice / slices * 2 * M_PI;
			float x = r * sin(theta);					
			float z = r * cos(theta);
			ponto.x = x;
			ponto.y = y;
			ponto.z = z;
			points[stack * (slices + 1) + slice] = ponto;
		}
	}

	//Triangles of the sphere
	for (int stack = 0; stack < stacks; ++stack)
	{
		for (int slice = 0; slice < slices; ++slice)
		{
			int topLeft = (stack + 0) * (slices + 1) + slice;
			int topRight = topLeft + 1;
			int bottomLeft = (stack + 1) * (slices + 1) + slice;
			int bottomRight = bottomLeft + 1;

			outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " " << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
			outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " " << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " " << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n"; 
		}
	}

	outFile.close();
}

void sphere(char *file, float radius, int slices, int stacks)
{
	generateSphere(file, radius, slices, stacks);
}
