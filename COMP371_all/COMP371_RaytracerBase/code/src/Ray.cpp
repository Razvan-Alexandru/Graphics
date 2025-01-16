#include "Ray.h"

Ray::Ray(const Eigen::Vector3f& origin, const Eigen::Vector3f& direction) {
    origin_ = origin;
    direction_ = direction.normalized();
}

Eigen::Vector3f Ray::at(const float& t) {
    return origin_ + t * direction_;
}