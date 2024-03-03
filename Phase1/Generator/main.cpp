#include <cstring>
#include <stdlib.h>

#include "figures/plane.h"
#include "figures/box.h"
#include "figures/sphere.h"
#include "figures/cone.h"

int main(int argc, char** argv) {

        if(strcmp(argv[1],"plane")==0){
            plane(argv[4], atof(argv[2]), atoi(argv[3]));
        }

        if(strcmp(argv[1],"box")==0){
            box(argv[4], atof(argv[2]), atoi(argv[3]));
        }

        if(strcmp(argv[1],"sphere")==0){
            sphere(argv[5],atof(argv[2]),atoi(argv[3]),atoi(argv[4]));
        }

        if(strcmp(argv[1],"cone")==0){
            cone(argv[6],atof(argv[2]),atof(argv[3]),atoi(argv[4]),atoi(argv[5]));

        }

    return 0;
}

