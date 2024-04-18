#include <cstring>
#include <stdlib.h>

#include "figures/plane.hpp"
#include "figures/box.hpp"
#include "figures/sphere.hpp"
#include "figures/cone.hpp"
#include "figures/bezier.hpp"

int main(int argc, char** argv) {

        if (strcmp(argv[1], "plane") == 0) plane(argv[4], atof(argv[2]), atoi(argv[3]));

        else if (strcmp(argv[1], "box") == 0) box(argv[4], atof(argv[2]), atoi(argv[3]));

        else if (strcmp(argv[1], "sphere") == 0) sphere(argv[5],atof(argv[2]),atoi(argv[3]),atoi(argv[4]));

        else if (strcmp(argv[1], "cone") == 0) cone(argv[6],atof(argv[2]),atof(argv[3]),atoi(argv[4]),atoi(argv[5]));

        else if (strcmp(argv[1], "patch") == 0) bezier(argv[2], atoi(argv[3]), argv[4]);

        else printf("Invalid figure\n");

    return 0;
}

// cd ../Generator && make && ./generator patch teapot.patch 10 bezier_10.3d
// cd ../Engine/ && make -C build && build/engine xml.xml