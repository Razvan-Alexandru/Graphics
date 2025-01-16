#include "Geometry.h"

#include <Eigen/Dense>

using namespace std;

Geometry::Geometry() {}
Geometry::Geometry(const nlohmann::json geo) {
    type = geo["type"];

    ka = geo["ka"];
    kd = geo["kd"];
    ks = geo["ks"];
    pc = geo["pc"];

    ac = {geo["ac"][0], geo["ac"][1], geo["ac"][2]};
    dc = {geo["dc"][0], geo["dc"][1], geo["dc"][2]};
    sc = {geo["sc"][0], geo["sc"][1], geo["sc"][2]};

    if(type == "rectangle") {
        p1 = Eigen::Vector3f(geo["p1"][0], geo["p1"][1], geo["p1"][2]);
        p2 = Eigen::Vector3f(geo["p2"][0], geo["p2"][1], geo["p2"][2]);
        p3 = Eigen::Vector3f(geo["p3"][0], geo["p3"][1], geo["p3"][2]);
        p4 = Eigen::Vector3f(geo["p4"][0], geo["p4"][1], geo["p4"][2]);

        normal = ((p2 - p1).cross(p3 - p1)).normalized();
    }

    if(type == "sphere") {
        centre = Eigen::Vector3f(geo["centre"][0], geo["centre"][1], geo["centre"][2]);
        radius = geo["radius"];
    }

    if(geo.contains("transform")) {
        //TODO
    }
}