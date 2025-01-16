#include "Scene.h"
#include <iostream>

Scene::Scene(const nlohmann::json j) {
    //parse json

    int nbrGeo = 0;
    int nbrLight = 0;
    int nbrOut = 0;

    for(auto geo : j["geometry"]) {
        if(geo.contains("visible")) { if(geo["visible"] == false) { continue; }}
        geometries.push_back(Geometry(geo));
        nbrGeo++;
    }

    for(auto light : j["light"]) {
        if(light.contains("use")) { if(light["use"] == false) { continue; }}
        lights.push_back(Light(light));
        nbrLight++;
    }

    for(auto out : j["output"]) {
        outputs.push_back(Output(out));
        nbrOut++;
    }

    cout << nbrGeo << " Geometry(ies)" << endl;
    cout << nbrLight << " Light(s)" << endl;
    cout << nbrOut << " Output(s)" << endl; 
}