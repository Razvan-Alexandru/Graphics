#include "Output.h"

#include <cmath>
#include <Eigen/Dense>

using namespace std;

Output::Output(const nlohmann::json out) {
    filename = out["filename"];
    size = {out["size"][0], out["size"][1]};
    
    fov = out["fov"];
    fov = fov * M_PI / 180;

    centre = Eigen::Vector3f(out["centre"][0], out["centre"][1], out["centre"][2]);
    up = Eigen::Vector3f(out["up"][0], out["up"][1], out["up"][2]);
    lookat = Eigen::Vector3f(out["lookat"][0], out["lookat"][1], out["lookat"][2]);

    ai = {out["ai"][0], out["ai"][1], out["ai"][2]};
    bkc = {out["bkc"][0], out["bkc"][1], out["bkc"][2]};

    rpp[0] = 1; rpp[1] = 1; rpp[2] = 10;
    if(out.contains("raysperpixel")) { 
        for(auto raysperpixelj : out["raysperpixel"]) {
            raysperpixel.push_back(raysperpixelj);
        }

        if (raysperpixel.size() == 1) { rpp[0] = 1; rpp[1] = 1; rpp[2] = raysperpixel[0]; }
        if (raysperpixel.size() == 2) { rpp[0] = raysperpixel[0]; rpp[1] = raysperpixel[0]; rpp[2] = raysperpixel[1]; }
        if (raysperpixel.size() == 3) { rpp[0] = raysperpixel[0]; rpp[1] = raysperpixel[1]; rpp[2] = raysperpixel[2]; }
    }

    antialiasing = false;
    if(out.contains("antialiasing")) {
        antialiasing = out["antialiasing"];
    }

    globalillum = false;
    if(out.contains("globalillum")) {
        globalillum = out["globalillum"];
    }

    twosiderender = false;
    if(out.contains("twosiderender")) {
        twosiderender = out["twosiderender"];
    }

    maxbounces = 0;
    if(out.contains("maxbounces")) {
        maxbounces = out["maxbounces"];
    }

    if(out.contains("probterminate")) {
        probterminates = out["probterminate"];
    }

    if(globalillum) {
        antialiasing = false;
    }

    if(antialiasing) {
        maxbounces = 0;
    }
}