#pragma once

#include <Eigen/Dense>

class Ray {
    private:
        Eigen::Vector3f origin_;
        Eigen::Vector3f direction_;
    
    public:
        Ray(const Eigen::Vector3f& origin, const Eigen::Vector3f& direction);

        Eigen::Vector3f at(const float& t);

        Eigen::Vector3f origin() const { return origin_; }
        Eigen::Vector3f direction() const { return direction_; }

        void setOrigin(Eigen::Vector3f o) { origin_ = o; }
        void setDirection(Eigen::Vector3f d) { direction_ = d.normalized();}
};