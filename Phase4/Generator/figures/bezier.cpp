#include "bezier.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>

struct Point {
    float x, y, z;
};

Point operator*(float scalar, const Point& point) {
    return {scalar * point.x, scalar * point.y, scalar * point.z};
}

Point operator+(const Point& a, const Point& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Point operator-(const Point& a, const Point& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

// Parse a single control point from a string
Point parsePoint(const std::string& line) {
    std::istringstream iss(line);
    Point p;
    char comma;  // Used to skip the comma
    iss >> p.x >> comma >> p.y >> comma >> p.z;
    return p;
}

// Calculate factorial
int factorial(int n) {
    return (n == 0 || n == 1) ? 1 : n * factorial(n - 1);
}

// Calculate binomial coefficient
float binomialCoeff(int n, int k) {
    return factorial(n) / (factorial(k) * factorial(n - k));
}

// Calculate a point on a Bezier patch using the Bernstein polynomial
Point bezierPatchPoint(const std::vector<Point>& controlPoints, int degree, float u, float v) {
    Point point = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i <= degree; ++i) {
        for (int j = 0; j <= degree; ++j) {
            float B_i = binomialCoeff(degree, i) * std::pow(u, i) * std::pow(1 - u, degree - i);
            float B_j = binomialCoeff(degree, j) * std::pow(v, j) * std::pow(1 - v, degree - j);
            point = point + (B_i * B_j * controlPoints[i * (degree + 1) + j]);
        }
    }
    return point;
}

// Calculate the tangent at a point on the Bezier patch in the u direction
Point bezierPatchTangentU(const std::vector<Point>& controlPoints, int degree, float u, float v) {
    Point tangent = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i <= degree; ++i) {
        for (int j = 0; j <= degree; ++j) {
            float B_i = binomialCoeff(degree, i) * (i * std::pow(u, i - 1) * std::pow(1 - u, degree - i) - (degree - i) * std::pow(u, i) * std::pow(1 - u, degree - i - 1));
            float B_j = binomialCoeff(degree, j) * std::pow(v, j) * std::pow(1 - v, degree - j);
            tangent = tangent + (B_i * B_j * controlPoints[i * (degree + 1) + j]);
        }
    }
    return tangent;
}

// Calculate the tangent at a point on the Bezier patch in the v direction
Point bezierPatchTangentV(const std::vector<Point>& controlPoints, int degree, float u, float v) {
    Point tangent = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i <= degree; ++i) {
        for (int j = 0; j <= degree; ++j) {
            float B_i = binomialCoeff(degree, i) * std::pow(u, i) * std::pow(1 - u, degree - i);
            float B_j = binomialCoeff(degree, j) * (j * std::pow(v, j - 1) * std::pow(1 - v, degree - j) - (degree - j) * std::pow(v, j) * std::pow(1 - v, degree - j - 1));
            tangent = tangent + (B_i * B_j * controlPoints[i * (degree + 1) + j]);
        }
    }
    return tangent;
}

// Calculate the cross product of two vectors
Point crossProduct(const Point& a, const Point& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Normalize a vector
Point normalize(const Point& p) {
    float length = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    if (length == 0) {
        return {0.0f, 0.0f, 0.0f};  // Return zero vector if length is zero
    }
    return {p.x / length, p.y / length, p.z / length};
}

// Write the Bezier patch to a file
void writeBezierPatch(const std::string& filename, const std::vector<std::vector<int>>& patches, const std::vector<Point>& controlPoints, int tessellation) {
    std::ofstream outFile("../Output/" + filename);
    if (!outFile.is_open()) {
        std::cerr << "Could not open file for output: " << filename << std::endl;
        return;
    }

    for (const auto& patch : patches) {
        for (int i = 0; i < tessellation; ++i) {
            float u = i / static_cast<float>(tessellation);
            float u_next = (i + 1) / static_cast<float>(tessellation);

            for (int j = 0; j < tessellation; ++j) {
                float v = j / static_cast<float>(tessellation);
                float v_next = (j + 1) / static_cast<float>(tessellation);

                std::vector<Point> patchPoints(16);
                for (int k = 0; k < 16; ++k) {
                    patchPoints[k] = controlPoints[patch[k]];
                }

                Point p1 = bezierPatchPoint(patchPoints, 3, u, v);
                Point p2 = bezierPatchPoint(patchPoints, 3, u, v_next);
                Point p3 = bezierPatchPoint(patchPoints, 3, u_next, v);
                Point p4 = bezierPatchPoint(patchPoints, 3, u_next, v_next);

                Point tangentU1 = bezierPatchTangentU(patchPoints, 3, u, v);
                Point tangentV1 = bezierPatchTangentV(patchPoints, 3, u, v);
                Point normal1 = normalize(crossProduct(tangentU1, tangentV1));

                Point tangentU2 = bezierPatchTangentU(patchPoints, 3, u, v_next);
                Point tangentV2 = bezierPatchTangentV(patchPoints, 3, u, v_next);
                Point normal2 = normalize(crossProduct(tangentU2, tangentV2));

                Point tangentU3 = bezierPatchTangentU(patchPoints, 3, u_next, v);
                Point tangentV3 = bezierPatchTangentV(patchPoints, 3, u_next, v);
                Point normal3 = normalize(crossProduct(tangentU3, tangentV3));

                Point tangentU4 = bezierPatchTangentU(patchPoints, 3, u_next, v_next);
                Point tangentV4 = bezierPatchTangentV(patchPoints, 3, u_next, v_next);
                Point normal4 = normalize(crossProduct(tangentU4, tangentV4));

                // First triangle
                if (normal1.x != normal1.x || normal2.x != normal2.x || normal3.x != normal3.x) continue;  // Skip NaN normals
                outFile << "n: " << normal1.x << "," << normal1.y << "," << normal1.z << "\n";
                outFile << "n: " << normal2.x << "," << normal2.y << "," << normal2.z << "\n";
                outFile << "n: " << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "t: " << p1.x << "," << p1.y << "," << p1.z << " "
                        << p2.x << "," << p2.y << "," << p2.z << " "
                        << p3.x << "," << p3.y << "," << p3.z << "\n";

                // Second triangle
                if (normal2.x != normal2.x || normal4.x != normal4.x || normal3.x != normal3.x) continue;  // Skip NaN normals
                outFile << "n: " << normal2.x << "," << normal2.y << "," << normal2.z << "\n";
                outFile << "n: " << normal4.x << "," << normal4.y << "," << normal4.z << "\n";
                outFile << "n: " << normal3.x << "," << normal3.y << "," << normal3.z << "\n";
                outFile << "t: " << p2.x << "," << p2.y << "," << p2.z << " "
                        << p4.x << "," << p4.y << "," << p4.z << " "
                        << p3.x << "," << p3.y << "," << p3.z << "\n";
            }
        }
    }
    outFile.close();
}

void bezier(char* controlPointsFile, int tessellationLevel, char* outputFileName) {
    std::ifstream inFile(controlPointsFile);
    if (!inFile) {
        std::cerr << "Failed to open control points file." << std::endl;
        return;
    }

    int numPatches;
    inFile >> numPatches;
    std::vector<std::vector<int>> patches(numPatches, std::vector<int>(16));

    // Skip the rest of the first line
    inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Read patch indices
    for (int i = 0; i < numPatches; ++i) {
        std::string line;
        std::getline(inFile, line);

        std::replace(line.begin(), line.end(), ',', ' ');

        std::istringstream iss(line);
        std::vector<int> patchIndices(std::istream_iterator<int>(iss), {});

        if (patchIndices.size() == 16) {
            patches[i] = patchIndices;
        } else {
            std::cerr << "Error: Patch indices size mismatch." << std::endl;
            return;
        }
    }

    // Read number of control points
    int numControlPoints;
    inFile >> numControlPoints;
    std::vector<Point> controlPoints(numControlPoints);

    // Skip the rest of the line
    inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Read control points
    for (int i = 0; i < numControlPoints; ++i) {
        std::string line;
        std::getline(inFile, line);
        controlPoints[i] = parsePoint(line);
    }

    inFile.close();

    // Generate and write the Bezier patch to the output file
    writeBezierPatch(outputFileName, patches, controlPoints, tessellationLevel);
}
