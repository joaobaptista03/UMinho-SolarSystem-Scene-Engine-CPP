#include "box.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void generateFace(std::ofstream &outFile, int divisions, float halfDimension, const std::string &axis, float normalDirection)
{
	float step = halfDimension * 2 / divisions;
	float **geral = new float *[(divisions + 1) * (divisions + 1)];

	// Generate all points of the face
	for (int i = 0; i <= divisions; ++i)
	{
		for (int j = 0; j <= divisions; ++j)
		{
			float x = -halfDimension + j * step;
			float y = halfDimension - i * step;
			float z = normalDirection * halfDimension;
			

			float *ponto = new float[3];
			if (axis == "xy")
			{
				ponto[0] = x;
				ponto[1] = y;
				ponto[2] = z;
			}
			else if (axis == "xz")
			{
				ponto[0] = x;
				ponto[1] = z;
				ponto[2] = y;
			}
			else if (axis == "yz")
			{
				ponto[0] = z;
				ponto[1] = x;
				ponto[2] = y;
			}

			geral[i * (divisions + 1) + j] = ponto;
		}
	}

	// Triangles of the face
	for (int i = 0; i < divisions; ++i)
	{
		for (int j = 0; j < divisions; ++j)
		{
			int topLeft = i * (divisions + 1) + j;
			int topRight = topLeft + 1;
			int bottomLeft = topLeft + (divisions + 1);
			int bottomRight = bottomLeft + 1;

			if (axis == "xy"){
				if (normalDirection == 1) 
				{
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << "\n";
				}
				else
				{
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << "\n";
				}
			}else if (axis == "xz"){
				if (normalDirection == 1)
				{
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << "\n";
				}
				else 
				{
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << "\n";
				}
			}else if (axis == "yz"){
				if (normalDirection == 1) 
				{
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << "\n";
				}
				else
				{
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[topRight][0] << "," << geral[topRight][1] << "," << geral[topRight][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << "\n";
					outFile << geral[topLeft][0] << "," << geral[topLeft][1] << "," << geral[topLeft][2] << " " << geral[bottomRight][0] << "," << geral[bottomRight][1] << "," << geral[bottomRight][2] << " " << geral[bottomLeft][0] << "," << geral[bottomLeft][1] << "," << geral[bottomLeft][2] << "\n";
				}
			}

		}
	}
}

// Function to generate all faces of the box
void generateBox(const std::string &fileName, float length, int divisions)
{

	std::string path = "../Output/" + fileName;
	std::ofstream outFile;
	outFile.open(path);

	if (!outFile.is_open())
	{
		std::cerr << "Failed to open file: " << path << "\n";
		return;
	}

	float halfDimension = length / 2.0f;

	// Each face of the box
	generateFace(outFile, divisions, halfDimension, "xy", 1);
	generateFace(outFile, divisions, halfDimension, "xy", -1); 
	generateFace(outFile, divisions, halfDimension, "xz", 1);
	generateFace(outFile, divisions, halfDimension, "xz", -1); 
	generateFace(outFile, divisions, halfDimension, "yz", 1);
	generateFace(outFile, divisions, halfDimension, "yz", -1); 

	outFile.close();
}
void box(char *file, float length, int divisions)
{
	generateBox(file, length, divisions);
}
