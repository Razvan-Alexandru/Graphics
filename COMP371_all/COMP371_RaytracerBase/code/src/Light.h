#pragma once

#include "../external/json.hpp"

#include <string>
#include <vector>
#include <Eigen/Dense>

using namespace std;

class Light{
    public:
        string type;
        vector<float> id, is;

        //point light
        Eigen::Vector3f centre;

        //area light
        Eigen::Vector3f p1, p2, p3, p4;
        Eigen::Vector3f xVec, yVec;

        //optional
        float n;
        bool usecentre, use;
        //transform

    public:
        Light(const nlohmann::json light);
};
