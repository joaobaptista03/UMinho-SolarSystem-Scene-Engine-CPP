#include "box.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void generateFace(std::ofstream& outFile, int divisions, float halfDimension, int& vertexOffset, const std::string& axis, float normalDirection) {
    float step = halfDimension * 2 / divisions;

    // Generate vertices
    for (int i = 0; i <= divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            float first = -halfDimension + j * step;
            float second = halfDimension - i * step; // Invert the second coordinate to start from the top
            float third = normalDirection * halfDimension;

            if (axis == "xy") {
                outFile << first << " " << second << " " << third << "\n";
            } else if (axis == "xz") {
                outFile << first << " " << third << " " << second << "\n";
            } else if (axis == "yz") {
                outFile << third << " " << first << " " << second << "\n";
            }
        }
    }

    // Generate indices for triangles
    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            int topLeft = vertexOffset + i * (divisions + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (divisions + 1);
            int bottomRight = bottomLeft + 1;

            outFile << topLeft << " " << bottomLeft << " " << bottomRight << "\n";
            outFile << topLeft << " " << bottomRight << " " << topRight << "\n";
        }
    }

    vertexOffset += (divisions + 1) * (divisions + 1); // Update the vertex offset for the next face
}

// Function to generate all faces of the box
void generateBox(const std::string& fileName, float length, int divisions) {
    
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return;
    }

    int vertexOffset = 0;
    float halfDimension = length / 2.0f;

    // Generate each face of the box
    generateFace(outFile, divisions, halfDimension, vertexOffset, "xy", 1);  // Front face (positive z)
    generateFace(outFile, divisions, halfDimension, vertexOffset, "xy", -1); // Back face (negative z)
    generateFace(outFile, divisions, halfDimension, vertexOffset, "xz", 1);  // Top face (positive y)
    generateFace(outFile, divisions, halfDimension, vertexOffset, "xz", -1); // Bottom face (negative y)
    generateFace(outFile, divisions, halfDimension, vertexOffset, "yz", 1);  // Right face (positive x)
    generateFace(outFile, divisions, halfDimension, vertexOffset, "yz", -1); // Left face (negative x)

    outFile.close();
}
void box(char*file, float length, int divisions){
    generateBox(file, length, divisions);
}
