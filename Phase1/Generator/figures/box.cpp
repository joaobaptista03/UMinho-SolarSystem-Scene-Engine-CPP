#include "box.h"

#include <iostream>
#include <fstream>
#include <vector>

void generateBox(const std::string& fileName, float length, int divisions) {
   
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);
    
    float halfLength = length / 2.0f;
    float step = length / divisions;



    for (int i = 0; i <= divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            // Top face (positive y)
            outFile << -halfLength + j * step << " " << halfLength << " " << -halfLength + i * step << "\n";
            // Bottom face (negative y)
            outFile << -halfLength + j * step << " " << -halfLength << " " << -halfLength + i * step << "\n";
        }
    }

    for (int i = 1; i < divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            // Front face (positive z)
            outFile << -halfLength + j * step << " " << -halfLength + i * step << " " << halfLength << "\n";
            // Back face (negative z)
            outFile << -halfLength + j * step << " " << -halfLength + i * step << " " << -halfLength << "\n";
            // Left face (negative x)
            outFile << -halfLength << " " << -halfLength + i * step << " " << -halfLength + j * step << "\n";
            // Right face (positive x)
            outFile << halfLength << " " << -halfLength + i * step << " " << -halfLength + j * step << "\n";
        }
    }
    
    outFile.close();

}

void box(char*file, float length, int divisions){
    generateBox(file, length, divisions);
}
