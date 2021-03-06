#pragma once

#include "Shape.h"

namespace raytracer {
namespace geometry {

class Plane : public Shape {
public:
  Plane() : Shape() {}

  std::vector<Intersection> LocalIntersect(utility::Ray &ray);
  utility::Vector LocalNormalAt(utility::Point &point);
};

} // namespace geometry
} // namespace raytracer
