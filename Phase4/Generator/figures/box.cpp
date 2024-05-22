#include "box.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

struct Point {
    float x, y, z;
};

void generateFace(std::ofstream &outFile, int divisions, float halfDimension, const std::string &axis, float normalDirection) {
    float step = halfDimension * 2 / divisions;
    std::vector<Point> points((divisions + 1) * (divisions + 1));

    for (int i = 0; i <= divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            float x = -halfDimension + j * step;
            float y = halfDimension - i * step;
            float z = normalDirection * halfDimension;

            Point ponto;
            if (axis == "xy") {
                ponto.x = x;
                ponto.y = y;
                ponto.z = z;
            } else if (axis == "xz") {
                ponto.x = x;
                ponto.y = z;
                ponto.z = y;
            } else if (axis == "yz") {
                ponto.x = z;
                ponto.y = x;
                ponto.z = y;
            }

            points[i * (divisions + 1) + j] = ponto;
        }
    }

    // Triangles of the face
    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            int topLeft = i * (divisions + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (divisions + 1);
            int bottomRight = bottomLeft + 1;

            Point normal1 = points[topLeft];
            Point normal2 = points[bottomLeft];
            Point normal3 = points[bottomRight];
            Point normal4 = points[topRight];

            // Normalize the normals
            auto normalize = [](const Point& p) -> Point {
                float length = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
                return {p.x / length, p.y / length, p.z / length};
            };

            normal1 = normalize(normal1);
            normal2 = normalize(normal2);
            normal3 = normalize(normal3);
            normal4 = normalize(normal4);


            // Ensure correct winding order for the triangles
            if (axis == "xy" && normalDirection == 1) { // Top face
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";

                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";

                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << "\n";

            } else if (axis == "xy" && normalDirection == -1) { // Bottom face
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";

                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << "\n";
            } else if (axis == "xz" && normalDirection == 1) { // Front face
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << "\n";
            } else if (axis == "xz" && normalDirection == -1) { // Back face
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << "\n";
            } else if (axis == "yz" && normalDirection == 1) { // Right face
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << "\n";
            } else if (axis == "yz" && normalDirection == -1) { // Left face
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[topRight].x << "," << points[topRight].y << "," << points[topRight].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << "\n";
                outFile << "t: " << points[topLeft].x << "," << points[topLeft].y << "," << points[topLeft].z << " "
                        << points[bottomRight].x << "," << points[bottomRight].y << "," << points[bottomRight].z << " "
                        << points[bottomLeft].x << "," << points[bottomLeft].y << "," << points[bottomLeft].z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal4.x << "," << normal4.y << "," << normal4.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << " ";
                outFile << normal3.x << "," << normal3.y << "," << normal3.z << " ";
                outFile << normal2.x << "," << normal2.y << "," << normal2.z << "\n";
            }
        }
    }
}

// Function to generate all faces of the box
void generateBox(const std::string &fileName, float length, int divisions) {
    std::string path = "../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return;
    }

    float halfDimension = length / 2.0f;

    // Each face of the box
    generateFace(outFile, divisions, halfDimension, "xy", 1);
    generateFace(outFile, divisions, halfDimension, "xy", -1);
    generateFace(outFile, divisions, halfDimension, "xz", 1);
    generateFace(outFile, divisions, halfDimension, "xz", -1);
    generateFace(outFile, divisions, halfDimension, "yz", 1);
    generateFace(outFile, divisions, halfDimension, "yz", -1);

    outFile.close();
}

void box(char *file, float length, int divisions) {
    generateBox(file, length, divisions);
}
