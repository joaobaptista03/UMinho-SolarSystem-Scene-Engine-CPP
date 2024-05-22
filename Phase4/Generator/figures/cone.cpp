#include "cone.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

// Struct to represent a point or vector in 3D space
struct Point {
    float x, y, z;
};

// Function to normalize a 3D vector
Point normalize3(const Point& p) {
    float length = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    return {p.x / length, p.y / length, p.z / length};
}

// Function to generate a cone and write to a file
void generateCone(const std::string &fileName, float radius, float height, int slices, int stacks) {
    std::filesystem::path outputPath = "../Output/" + fileName;

    // Ensure the output directory exists
    std::filesystem::create_directories(outputPath.parent_path());

    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << "\n";
        return;
    }

    std::vector<Point> points((stacks + 1) * slices + 2);
    Point apex = {0, height, 0};
    points[0] = apex;

    // Base
    for (int slice = 0; slice < slices; ++slice) {
        float theta = (2.0f * M_PI * slice) / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        points[slice + 1] = {x, 0, z};
    }

    // Sides
    for (int stack = 1; stack < stacks; ++stack) {
        float y = height * stack / stacks;
        float r = radius * (1 - (float)stack / stacks);
        for (int slice = 0; slice < slices; ++slice) {
            float theta = (float)slice / slices * 2 * M_PI;
            float x = r * cos(theta);
            float z = r * sin(theta);
            points[stack * slices + slice + 1] = {x, y, z};
        }
    }

    Point origin = {0, 0, 0};
    points[stacks * slices + 1] = origin;

    int apexIndex = 0;
    int baseStartIndex = 1;
    int originIndex = stacks * slices + 1;

    // Sides triangles with normals
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int bottomRight = baseStartIndex + (stack * slices) + slice;
            int bottomLeft = baseStartIndex + (stack * slices) + ((slice + 1) % slices);
            int topLeft = baseStartIndex + ((stack + 1) * slices) + ((slice + 1) % slices);
            int topRight = baseStartIndex + ((stack + 1) * slices) + slice;

            if (stack == stacks - 1) {
                Point normal1 = normalize3(points[apexIndex]);
                Point normal2 = normalize3(points[bottomLeft]);
                Point normal3 = normalize3(points[bottomRight]);

                outFile << "t: " << points[apexIndex].x << "," << points[apexIndex].y << "," << points[apexIndex].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
            } else {
                Point normal1 = normalize3(points[topLeft]);
                Point normal2 = normalize3(points[bottomLeft]);
                Point normal3 = normalize3(points[bottomRight]);
                Point normal4 = normalize3(points[topRight]);

                // First triangle
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";

                // Second triangle
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << "\n";
            }
        }
    }

    // Base triangles with normals
    for (int slice = 1; slice < slices; ++slice) {
        Point normal = normalize3(points[originIndex]);

        outFile << "t: " << points[slice].x << "," << points[slice].y << "," << points[slice].z << " "
                << points[(slice + 1)].x << "," << points[(slice + 1)].y << "," << points[(slice + 1)].z << " "
                << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";
        outFile << "n: " << normal.x << "," << normal.y << "," << normal.z << " ";
        outFile << normal.x << "," << normal.y << "," << normal.z << " ";
        outFile << normal.x << "," << normal.y << "," << normal.z << "\n";
    }
    Point normal = normalize3(points[originIndex]);
    outFile << "t: " << points[slices].x << "," << points[slices].y << "," << points[slices].z << " "
            << points[1].x << "," << points[1].y << "," << points[1].z << " "
            << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";
    outFile << "n: " << normal.x << "," << normal.y << "," << normal.z << " ";
    outFile << normal.x << "," << normal.y << "," << normal.z << " ";
    outFile << normal.x << "," << normal.y << "," << normal.z << "\n";

    outFile.close();
}

// Wrapper function for easier usage
void cone(char *file, float radius, float height, int slices, int stacks) {
    generateCone(file, radius, height, slices, stacks);
}
