#pragma once

#include "../external/json.hpp"

#include <string>
#include <vector>
#include <Eigen/Dense>

using namespace std;

class Output{
    public:
        string filename;
        vector<unsigned int> size;
        float fov;
        Eigen::Vector3f centre, up, lookat;
        vector<float> bkc, ai;

        //optional
        vector<unsigned int> raysperpixel;
        int rpp[3];
        bool antialiasing, twosiderender, globalillum;
        int maxbounces;
        float probterminates;

    public:
        Output(const nlohmann::json out);
};