#include <iostream>
#include <fstream>
#include <vector>

void generateFace(std::ofstream& outFile, int divisions, float halfDimension, const std::string& axis, float normalDirection);
void generateBox(const std::string& fileName, float length, int divisions);
void box(char*file, float length, int divisions);