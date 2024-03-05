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

	if (!outFile.is_open())
	{
		std::cerr << "Error opening file: " << fileName << "\n";
		return;
	}

	outFile << "0," << height << ",0"
			<< "," << std::endl; // Tip of the cone is now the first vertex

	// Generate vertices for the base of the cone
	for (int slice = 0; slice < slices; ++slice)
	{
		float theta = (2.0f * M_PI * slice) / slices;
		float x = radius * cos(theta);
		float z = radius * sin(theta);
		outFile << x << "," << 0 << "," << z << "," << std::endl; // y = 0 for the base vertices
	}

	// Generate vertices for the sides of the cone
	for (int stack = 1; stack < stacks; ++stack)
	{ // Start from 1 to skip the base center and apex
		float y = height * stack / stacks;
		float r = radius * (1 - (float)stack / stacks);
		for (int slice = 0; slice < slices; ++slice)
		{
			float theta = (float)slice / slices * 2 * M_PI;
			float x = r * cos(theta);
			float z = r * sin(theta);
			outFile << x << "," << y << "," << z << "," << std::endl;
		}
	}

	outFile << "0,"
			<< "0"
			<< ",0"
			<< "," << std::endl; // Tip of the cone is now the first vertex

	int apexIndex = 0;
	int baseStartIndex = 1;
	int origin = stacks * slices + 1;

	// Generate triangles for the sides of the cone
	for (int stack = 0; stack < stacks; ++stack)
	{
		for (int slice = 0; slice < slices; ++slice)
		{
			// Calculate the four corners of the "square"
			int bottomRight = baseStartIndex + (stack * slices) + slice;
			int bottomLeft = baseStartIndex + (stack * slices) + ((slice + 1) % slices);
			int topLeft = baseStartIndex + ((stack + 1) * slices) + ((slice + 1) % slices);
			int topRight = baseStartIndex + ((stack + 1) * slices) + slice;

			// The triangles for the side
			if (stack == 2)
			{ // If it's the first stack, connect to the apex
				outFile << apexIndex << "," << bottomLeft << "," << bottomRight << "," << std::endl;
			}
			else
			{ // For stacks between the apex and base
				outFile << topLeft << "," << bottomLeft << "," << bottomRight << "," << std::endl;
				outFile << topLeft << "," << bottomRight << "," << topRight << "," << std::endl;
			}
		}
	}

	// Generate triangles for the base of the cone
	for (int slice = 1; slice < slices; ++slice)
	{
		outFile << slice << "," << (slice + 1) << "," << origin << "," << std::endl;
	}
	outFile << slices << "," << 1 << "," << origin << "," << std::endl;

	outFile.close();
}

void cone(char *file, float radius, float height, int slices, int stacks)
{
	generateCone(file, radius, height, slices, stacks);
}