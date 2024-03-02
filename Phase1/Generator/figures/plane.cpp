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
    int y = 0;


    for(int i = 0; i < divisions; i++) {
        for(int j = 0; j < divisions; j++) {
            float x = -halfLength + j * divisionSize;
            float z = -halfLength + i * divisionSize;
            outFile << x << "," << y << "," << z << "\n";
        }
    }

    outFile.close();

}

void plane(char*file, float length, int divisions){
    generatePlane(file, length, divisions);
}
