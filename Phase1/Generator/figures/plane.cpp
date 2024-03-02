#include "plane.h"

#include <iostream>
#include <fstream>
#include <vector>

void generatePlane(const std::string& fileName, float length, int divisions) {
   
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);
    
    float halfLength = length / 2.0f;
    float divisionSize = length / divisions;
    int verticesPerLine = divisions + 1;

    // Write vertices
    for (int i = 0; i <= divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            float x = -halfLength + j * divisionSize;
            float z = -halfLength + i * divisionSize;
            outFile << x << " " << 0 << " " << z << std::endl;
        }
    }
/*
    // Write triangle definitions using the indices of the vertices
    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            int topLeft = i * verticesPerLine + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * verticesPerLine + j;
            int bottomRight = bottomLeft + 1;

            // First triangle
            outFile << topLeft << " " << bottomLeft << " " << topRight << std::endl;
            // Second triangle
            outFile << topRight << " " << bottomLeft << " " << bottomRight << std::endl;
        }
    }
*/
    outFile.close();

}

void plane(char*file, float length, int divisions){
    generatePlane(file, length, divisions);
}
