#pragma once

#include "../external/json.hpp"

#include <string>
#include <vector>
#include <Eigen/Dense>

using namespace std;

class Geometry {
    public:
        string type;
        float ka, kd, ks, pc;
        vector<float> ac, dc, sc;

        //rectangle
        Eigen::Vector3f p1, p2, p3, p4;
        Eigen::Vector3f normal;

        //sphere
        Eigen::Vector3f centre;
        float radius;

        //optional
        bool visible;

        //transform
        //HERE

    public:
        Geometry();
        Geometry(const nlohmann::json geo);
};