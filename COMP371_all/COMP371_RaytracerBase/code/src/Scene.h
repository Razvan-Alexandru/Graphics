#pragma once

#include <vector>
#include "Geometry.h"
#include "Light.h"
#include "Output.h"

using namespace std;

class Scene {
    public:
        vector<Geometry> geometries;
        vector<Light> lights;
        vector<Output> outputs;

    public:
        Scene(const nlohmann::json j);
};
