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
Point normalize3(const Point& p1, const Point& p2, const Point& p3) {

    Point v1 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
    Point v2 = {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z};

    // Cross product of two vectors
    Point normal = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };

    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    return {normal.x / length, normal.y / length, normal.z / length};
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

    // Sides triangles with normals and texture coordinates
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int bottomRight = baseStartIndex + (stack * slices) + slice;
            int bottomLeft = baseStartIndex + (stack * slices) + ((slice + 1) % slices);
            int topLeft = baseStartIndex + ((stack + 1) * slices) + ((slice + 1) % slices);
            int topRight = baseStartIndex + ((stack + 1) * slices) + slice;

            float u1 = (float)slice / slices;
            float u2 = (float)(slice + 1) / slices;
            float v1 = (float)stack / stacks;
            float v2 = (float)(stack + 1) / stacks;

            if (stack == stacks - 1) {
                // ApexIndex corresponds to the apex of the cone, which is the top of the cone (coor)
                Point normal1 = normalize3(points[bottomLeft], points[bottomRight], points[apexIndex]);
                Point normal2 = normalize3(points[bottomLeft], points[bottomRight], points[apexIndex]);
                Point normal3 = normalize3(points[bottomLeft], points[bottomRight], points[apexIndex]);

                outFile << "t: " << points[apexIndex].x << "," << points[apexIndex].y << "," << points[apexIndex].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "v: " << 0.5 << "," << 1.0 << " " << u2 << "," << v1 << " " << u1 << "," << v1 << "\n";
            } else {
                Point normal1 = normalize3(points[topLeft], points[bottomLeft], points[topRight]);
                Point normal2 = normalize3(points[topLeft], points[bottomLeft], points[topRight]);
                Point normal3 = normalize3(points[topLeft], points[bottomLeft], points[topRight]);
                Point normal4 = normalize3(points[topLeft], points[bottomLeft], points[topRight]);

                // First triangle
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "v: " << u2 << "," << v2 << " " << u2 << "," << v1 << " " << u1 << "," << v1 << "\n";

                // Second triangle
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << "\n";
                outFile << "v: " << u2 << "," << v2 << " " << u1 << "," << v1 << " " << u1 << "," << v2 << "\n";
            }
        }
    }

    // Base triangles with normals and texture coordinates
    for (int slice = 1; slice < slices; ++slice) {

        float u1 = (float)slice / slices;
        float u2 = (float)(slice + 1) / slices;

        outFile << "t: " << points[slice].x << "," << points[slice].y << "," << points[slice].z << " "
                << points[(slice + 1)].x << "," << points[(slice + 1)].y << "," << points[(slice + 1)].z << " "
                << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";
        outFile << "n: " << 0 << "," << -1 << "," << 0 << " ";
        outFile << 0 << "," << -1 << "," << 0 << " ";
        outFile << 0 << "," << -1 << "," << 0 << "\n";
        outFile << "v: " << u1 << "," << 0.0 << " " << u2 << "," << 0.0 << " " << (u1 + u2) / 2 << "," << 1.0 << "\n";
    }
    outFile << "t: " << points[slices].x << "," << points[slices].y << "," << points[slices].z << " "
            << points[1].x << "," << points[1].y << "," << points[1].z << " "
            << points[originIndex].x << "," << points[originIndex].y << "," << points[originIndex].z << "\n";
    outFile << "n: " << 0 << "," << -1 << "," << 0 << " ";
    outFile << 0 << "," << -1 << "," << 0 << " ";
    outFile << 0 << "," << -1 << "," << 0 << "\n";
    outFile << "v: " << 1.0 << "," << 0.0 << " " << 0.0 << "," << 0.0 << " " << 0.5 << "," << 1.0 << "\n";

    outFile.close();
}

// Wrapper function for easier usage
void cone(char *file, float radius, float height, int slices, int stacks) {
    generateCone(file, radius, height, slices, stacks);
}
