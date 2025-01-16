#pragma once

#include <iostream>
#include "../external/json.hpp"

#include "Scene.h"

class RayTracer {
    private:
        Scene scene;

    public:
        RayTracer(const nlohmann::json j);
        void run();

        vector<double> renderDirectIllumination(Output& output);
        vector<double> renderAntiAliasing(Output& output);
        vector<double> renderGlobalIllumination(Output& output);
};
