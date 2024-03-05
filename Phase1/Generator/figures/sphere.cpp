#include "sphere.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void generateSphere(const std::string &fileName, float radius, int slices, int stacks)
{

	std::string path = "../Output/" + fileName;
	std::ofstream outFile;
	outFile.open(path);
	float** geral = new float*[slices * stacks * 2];

	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file: " << path << "\n";
		return;
	}

	for (int stack = 0; stack <= stacks; ++stack)
	{
		float phi = M_PI / 2 - (float)stack / stacks * M_PI; // From pi/2 to -pi/2
		float y = radius * sin(phi);						 // y is the same for each slice on a stack
		float r = radius * cos(phi);

		for (int slice = 0; slice <= slices; ++slice)
		{
			float *ponto = new float[3];
			float theta = (float)slice / slices * 2 * M_PI; // From 0 to 2pi
			float x = r * sin(theta);						// x, z vary with slice
			float z = r * cos(theta);
			ponto[0] = x;
			ponto[1] = y;
			ponto[2] = z;
			geral[stack * (slices + 1) + slice] = ponto;
		}
	}

	for (int stack = 0; stack < stacks; ++stack)
	{
		for (int slice = 0; slice < slices; ++slice)
		{
			int topLeft = (stack + 0) * (slices + 1) + slice;
			int topRight = topLeft + 1;
			int bottomLeft = (stack + 1) * (slices + 1) + slice;
			int bottomRight = bottomLeft + 1;

			outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
			outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << "\n";
		}
	}

	outFile.close();
}

void sphere(char *file, float radius, int slices, int stacks)
{
	generateSphere(file, radius, slices, stacks);
}
