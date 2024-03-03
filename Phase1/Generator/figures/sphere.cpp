#include "sphere.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void generateSphere(const std::string& fileName, float radius, int slices, int stacks){

    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return;
    }

    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = M_PI / 2 - (float)stack / stacks * M_PI; // From pi/2 to -pi/2
        float y = radius * sin(phi);                          // y is the same for each slice on a stack
        float r = radius * cos(phi);

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = (float)slice / slices * 2 * M_PI; // From 0 to 2pi
            float x = r * sin(theta);                       // x, z vary with slice
            float z = r * cos(theta);
            outFile << x << "," << y << "," << z << "," << "\n";
        }
    }

    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int topLeft = (stack + 0) * (slices + 1) + slice;
            int topRight = topLeft + 1;
            int bottomLeft = (stack + 1) * (slices + 1) + slice;
            int bottomRight = bottomLeft + 1;

            // Write indices for the top triangle of the quad
            outFile << topLeft << "," << bottomLeft << "," << bottomRight << "," << "\n";
            // Write indices for the bottom triangle of the quad
            outFile << topLeft << "," << bottomRight << "," << topRight << "," << "\n";
        }
    }

    outFile.close();
}


void sphere(char*file, float radius, int slices, int stacks){
    generateSphere(file, radius, slices, stacks);
}
