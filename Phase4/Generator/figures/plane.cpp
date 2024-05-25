#include "plane.hpp"

#include <iostream>
#include <fstream>
#include <vector>

struct Point {
    float x, y, z;
};

void generatePlane(const std::string &fileName, float length, int divisions) {
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return;
    }

    float halfLength = length / 2.0f;
    float divisionSize = length / divisions;
    int verticesPerLine = divisions + 1;
    std::vector<Point> points;

    for (int i = 0; i <= divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            float x = -halfLength + j * divisionSize;
            float z = -halfLength + i * divisionSize;
            Point ponto;
            ponto.x = x;
            ponto.y = 0;
            ponto.z = z;
            points.push_back(ponto);
        }
    }

    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            int topLeft = i * verticesPerLine + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * verticesPerLine + j;
            int bottomRight = bottomLeft + 1;

            // Texture coordinates
            float u1 = (float)j / divisions;
            float u2 = (float)(j + 1) / divisions;
            float v1 = (float)i / divisions;
            float v2 = (float)(i + 1) / divisions;

            // First triangle
            outFile << "t: " 
                    << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                    << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                    << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";

            outFile << "n: " << 0 << "," << 1 << "," << 0 << " ";
            outFile << 0 << "," << 1 << "," << 0 << " ";
            outFile << 0 << "," << 1 << "," << 0 << "\n";

            outFile << "v: " << u1 << "," << v1 << " " << u1 << "," << v2 << " " << u2 << "," << v2 << "\n";

            // Second triangle
            outFile << "t: " 
                    << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                    << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                    << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";

            outFile << "n: " << 0 << "," << 1 << "," << 0 << " ";
            outFile << 0 << "," << 1 << "," << 0 << " ";
            outFile << 0 << "," << 1 << "," << 0 << "\n";

            outFile << "v: " << u1 << "," << v1 << " " << u2 << "," << v2 << " " << u2 << "," << v1 << "\n";
        }
    }

    outFile.close();
}

void plane(char *file, float length, int divisions) {
    generatePlane(file, length, divisions);
}
