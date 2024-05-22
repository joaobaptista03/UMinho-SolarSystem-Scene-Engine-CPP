#include "sphere.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Point {
    float x, y, z;
};

struct Normal {
    float x, y, z;
};

void generateSphere(const std::string &fileName, float radius, int slices, int stacks) {
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);
    std::vector<Point> points((stacks + 1) * (slices + 1));

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return;
    }

    // Vertices of the sphere
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = M_PI / 2 - static_cast<float>(stack) / stacks * M_PI;
        float y = radius * sin(phi);
        float r = radius * cos(phi);

        for (int slice = 0; slice <= slices; ++slice) {
            Point point;
            float theta = static_cast<float>(slice) / slices * 2 * M_PI;
            float x = r * sin(theta);
            float z = r * cos(theta);
            point.x = x;
            point.y = y;
            point.z = z;
            points[stack * (slices + 1) + slice] = point;
        }
    }

    // Triangles of the sphere with normals
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int topLeft = stack * (slices + 1) + slice;
            int topRight = topLeft + 1;
            int bottomLeft = (stack + 1) * (slices + 1) + slice;
            int bottomRight = bottomLeft + 1;

            Point normal1 = points[topLeft];
            Point normal2 = points[bottomLeft];
            Point normal3 = points[bottomRight];
            Point normal4 = points[topRight];

            // Write first triangle
            outFile << "n: " << normal1.x / radius << "," << normal1.y / radius << "," << normal1.z / radius << "\n";
            outFile << "n: " << normal2.x / radius << "," << normal2.y / radius << "," << normal2.z / radius << "\n";
            outFile << "n: " << normal3.x / radius << "," << normal3.y / radius << "," << normal3.z / radius << "\n";
            outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                    << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                    << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";

            // Write second triangle
            outFile << "n: " << normal1.x / radius << "," << normal1.y / radius << "," << normal1.z / radius << "\n";
            outFile << "n: " << normal3.x / radius << "," << normal3.y / radius << "," << normal3.z / radius << "\n";
            outFile << "n: " << normal4.x / radius << "," << normal4.y / radius << "," << normal4.z / radius << "\n";
            outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                    << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                    << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";
        }
    }

    outFile.close();
}

void sphere(char *file, float radius, int slices, int stacks) {
    generateSphere(file, radius, slices, stacks);
}
