#include "plane.h"

#include <iostream>
#include <fstream>
#include <vector>

void generatePlane(const std::string& fileName, float length, int divisions) {
   
    std::string path = "../../Output/" + fileName;
    std::ofstream outFile;
    outFile.open(path);
    
    //Execuatr aqui a escrita do ficheiro

    outFile.close();

}

void plane(char*file, float length, int divisions){
    generatePlane(file, length, divisions);
}
