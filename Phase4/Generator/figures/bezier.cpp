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

// Parse a single control point from a string
Point parsePoint(const std::string& line) {
    std::istringstream iss(line);
    Point p;
    char comma;  // Used to skip the comma
    iss >> p.x >> comma >> p.y >> comma >> p.z;
    return p;
}

// Calculate a point on a Bezier patch using the Bernstein polynomial
Point bezierPatchPoint(const std::vector<Point>& controlPoints, int degree, float u, float v) {
    Point point = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i <= degree; ++i) {
        for (int j = 0; j <= degree; ++j) {
            float B_i = std::tgamma(degree + 1) / (std::tgamma(i + 1) * std::tgamma(degree - i + 1)) * std::pow(u, i) * std::pow(1 - u, degree - i);
            float B_j = std::tgamma(degree + 1) / (std::tgamma(j + 1) * std::tgamma(degree - j + 1)) * std::pow(v, j) * std::pow(1 - v, degree - j);
            point = point + (B_i * B_j * controlPoints[i * (degree + 1) + j]);
        }
    }
    return point;
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

                // First triangle
                outFile << "t: " << p1.x << "," << p1.y << "," << p1.z << " ";
                outFile << p2.x << "," << p2.y << "," << p2.z << " ";
                outFile << p3.x << "," << p3.y << "," << p3.z << "\n";

                // Second triangle
                outFile << "t: " << p2.x << "," << p2.y << "," << p2.z << " ";
                outFile << p4.x << "," << p4.y << "," << p4.z << " ";
                outFile << p3.x << "," << p3.y << "," << p3.z << "\n";
            }
        }
    }
    outFile.close();
}

void bezier(char *controlPointsFile, int tessellationLevel, char *outputFileName) {
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
            std::copy(patchIndices.begin(), patchIndices.end(), std::ostream_iterator<int>(std::cout, " "));
            std::cout << std::endl;
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
