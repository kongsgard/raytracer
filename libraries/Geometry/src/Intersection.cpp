#include "Intersection.h"

#include <algorithm>

#include "AlmostEquals.h"

namespace raytracer {
namespace geometry {

Computations Intersection::PrepareComputations(
    utility::Ray ray, const std::vector<Intersection> &intersections) {
  Computations comps;

  std::vector<std::shared_ptr<Shape>> containers = {};
  for (auto i : intersections) {
    if (i == *this) {
      if (containers.empty()) {
        comps.n1 = 1.0;
      } else {
        comps.n1 = containers.back().get()->material_.refractive_index_;
      }
    }

    const auto id = i.object_.get()->id_;
    auto it = std::find_if(
        containers.begin(), containers.end(),
        [id](std::shared_ptr<Shape> const &i) { return i.get()->id_ == id; });
    if (it != containers.end()) {
      containers.erase(it);
    } else {
      containers.push_back(i.object_);
    }

    if (i == *this) {
      if (containers.empty()) {
        comps.n2 = 1.0;
      } else {
        comps.n2 = containers.back().get()->material_.refractive_index_;
      }
    }
  }

  // Copy the intersection's properties, for convenience
  comps.t = t_;
  comps.object = object_;

  // Precompute some useful values
  comps.point = ray.Position(comps.t);
  comps.eye_vector = -ray.direction_;
  comps.normal_vector = comps.object->NormalAt(comps.point);

  if (comps.normal_vector.Dot(comps.eye_vector) < 0) {
    comps.inside = true;
    comps.normal_vector = -comps.normal_vector;
  } else {
    comps.inside = false;
  }

  comps.reflect_vector = ray.direction_.Reflect(comps.normal_vector);

  comps.over_point = comps.point + comps.normal_vector * EPSILON;
  comps.under_point = comps.point - comps.normal_vector * EPSILON;

  return comps;
}

std::vector<Intersection>
Intersections(const std::initializer_list<Intersection> &intersections) {
  std::vector<Intersection> intersections_vector;
  for (auto intersection : intersections) {
    intersections_vector.push_back(intersection);
  }
  sort(intersections_vector.begin(), intersections_vector.end(),
       [](const auto &lhs, const auto &rhs) { return lhs.t_ < rhs.t_; });
  return intersections_vector;
}

std::optional<Intersection>
Hit(const std::vector<Intersection> &intersections) {
  for (auto &i : intersections) {
    if (i.t_ > 0) {
      return i;
    }
  }
  return std::nullopt;
}

} // namespace geometry
} // namespace raytracer

bool operator==(const raytracer::geometry::Intersection &i1,
                const raytracer::geometry::Intersection &i2) {
  return AlmostEquals(i1.t_, i2.t_) && i1.object_->id_ == i2.object_->id_;
}

bool operator!=(const raytracer::geometry::Intersection &i1,
                const raytracer::geometry::Intersection &i2) {
  return !(i1 == i2);
}
