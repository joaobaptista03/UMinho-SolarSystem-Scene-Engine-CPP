#include "cone.hpp"
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

Point calculateNormal(Point a, Point b, Point c) {
    // Calculate the normal of the triangle defined by points a, b, and c
    float x1 = b.x - a.x;
    float y1 = b.y - a.y;
    float z1 = b.z - a.z;
    float x2 = c.x - a.x;
    float y2 = c.y - a.y;
    float z2 = c.z - a.z;

    float nx = y1 * z2 - z1 * y2;
    float ny = z1 * x2 - x1 * z2;
    float nz = x1 * y2 - y1 * x2;

    float length = std::sqrt(nx * nx + ny * ny + nz * nz);
    return {nx / length, ny / length, nz / length};
}

void generateCone(const std::string &fileName, float radius, float height, int slices, int stacks) {
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);
    std::vector<Point> points(stacks * slices + slices + 2); // apex, stacks * slices, base points, origin

    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << "\n";
        return;
    }

    Point apex = {0, height, 0};
    points[0] = apex;

    // Generate base points
    for (int slice = 0; slice < slices; ++slice) {
        float theta = (2.0f * M_PI * slice) / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        Point point;
        point.x = x;
        point.y = 0;
        point.z = z;
        points[slice + 1] = point;
    }

    // Generate side points for each stack
    for (int stack = 1; stack < stacks; ++stack) {
        float y = height * stack / stacks;
        float r = radius * (1 - static_cast<float>(stack) / stacks);
        for (int slice = 0; slice < slices; ++slice) {
            float theta = static_cast<float>(slice) / slices * 2 * M_PI;
            float x = r * cos(theta);
            float z = r * sin(theta);
            Point point;
            point.x = x;
            point.y = y;
            point.z = z;
            points[stack * slices + slice + 1] = point;
        }
    }

    Point origin = {0, 0, 0};
    points[stacks * slices + slices + 1] = origin;

    int apexIndex = 0;
    int baseStartIndex = 1;
    int originIndex = stacks * slices + slices + 1;

    // Generate side triangles
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int bottomRight = baseStartIndex + (stack * slices) + slice;
            int bottomLeft = baseStartIndex + (stack * slices) + ((slice + 1) % slices);
            int topLeft = baseStartIndex + ((stack + 1) * slices) + ((slice + 1) % slices);
            int topRight = baseStartIndex + ((stack + 1) * slices) + slice;

            if (stack == stacks - 1) {
                Point normal = calculateNormal(points[apexIndex], points[bottomLeft], points[bottomRight]);
                outFile << "n: " << normal.x << "," << normal.y << "," << normal.z << "\n";
                outFile << "t: " << points[apexIndex].x << "," << points[apexIndex].y << "," << points[apexIndex].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
            } else {
                Point normal = calculateNormal(points[topLeft], points[bottomLeft], points[bottomRight]);
                outFile << "n: " << normal.x << "," << normal.y << "," << normal.z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "n: " << normal.x << "," << normal.y << "," << normal.z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";
            }
        }
    }

    // Generate base triangles
    Point baseNormal = {0, -1, 0};
    for (int slice = 1; slice < slices; ++slice) {
        outFile << "n: " << baseNormal.x << "," << baseNormal.y << "," << baseNormal.z << "\n";
        outFile << "t: " << points[slice].x << "," << points[slice].y << "," << points[slice].z << " "
                << points[slice + 1].x << "," << points[slice + 1].y << "," << points[slice + 1].z << " "
                << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";
    }
    outFile << "n: " << baseNormal.x << "," << baseNormal.y << "," << baseNormal.z << "\n";
    outFile << "t: " << points[slices].x << "," << points[slices].y << "," << points[slices].z << " "
            << points[1].x << "," << points[1].y << "," << points[1].z << " "
            << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";

    outFile.close();
}

void cone(char *file, float radius, float height, int slices, int stacks) {
    generateCone(file, radius, height, slices, stacks);
}