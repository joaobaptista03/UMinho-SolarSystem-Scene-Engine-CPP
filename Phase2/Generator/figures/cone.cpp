#include "cone.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void generateCone(const std::string &fileName, float radius, float height, int slices, int stacks)
{
	std::string path = "../Output/" + fileName;
	std::ofstream outFile;
	outFile.open(path);
	float** geral = new float*[slices * stacks * 2];

	if (!outFile.is_open())
	{
		std::cerr << "Error opening file: " << fileName << "\n";
		return;
	}

	float *apex = new float[3];
	apex[0] = 0;
	apex[1] = height;
	apex[2] = 0;
	geral[0] = apex;

	//Base
	for (int slice = 0; slice < slices; ++slice)
	{
		float theta = (2.0f * M_PI * slice) / slices;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		float *ponto = new float[3];
		ponto[0] = x;
		ponto[1] = 0;
		ponto[2] = z;
		geral[slice + 1] = ponto;
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
			float *ponto = new float[3];
			ponto[0] = x;
			ponto[1] = y;
			ponto[2] = z;
			geral[stack * slices + slice + 1] = ponto;
		}
	}


	float *origin = new float[3];
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	geral[stacks * slices + 1] = origin;


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
				outFile << geral[apexIndex][0] << "," << geral[apexIndex][1] << "," << geral[apexIndex][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
			}
			else
			{

				outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
				outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << "\n";

			}
		}
	}

	//Base triangles
	for (int slice = 1; slice < slices; ++slice)
	{
		outFile << geral[slice][0] << "," << geral[slice][1] << "," << geral[slice][2] << " " << geral[(slice + 1)][0] << "," << geral[(slice + 1)][1] << "," << geral[(slice + 1)][2] << " " << geral[originIndex][0] << "," << geral[originIndex][1] << "," << geral[originIndex][2] << "\n";

	}
	outFile << geral[slices][0] << "," << geral[slices][1] << "," << geral[slices][2] << " " << geral[1][0] << "," << geral[1][1] << "," << geral[1][2] << " " << geral[originIndex][0] << "," << geral[originIndex][1] << "," << geral[originIndex][2] << "\n";

	for (int i = 0; i < slices * stacks * 2; i++)
	{
		delete[] geral[i];
	}

	outFile.close();
}

void cone(char *file, float radius, float height, int slices, int stacks)
{
	generateCone(file, radius, height, slices, stacks);
}