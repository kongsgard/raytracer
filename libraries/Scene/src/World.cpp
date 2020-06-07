#include "World.h"

namespace raytracer {
namespace scene {

const bool World::Contains(const PointLight light) const {
  for (const auto &i : light_sources_) {
    if (*i == light) {
      return true;
    }
  }
  return false;
}

const bool
World::Contains(const std::shared_ptr<geometry::Shape> object) const {
  for (const auto &i : objects_) {
    if (*i == *object) {
      return true;
    }
  }
  return false;
}

std::vector<geometry::Intersection> World::Intersect(utility::Ray ray) {
  std::vector<geometry::Intersection> intersections_vector;
  for (auto &i : objects_) {
    std::vector<geometry::Intersection> xs = i->Intersect(ray);
    intersections_vector.insert(std::end(intersections_vector), std::begin(xs),
                                std::end(xs));
  }
  sort(intersections_vector.begin(), intersections_vector.end(),
       [](const auto &lhs, const auto &rhs) { return lhs.t_ < rhs.t_; });
  return intersections_vector;
}

utility::Color World::ShadeHit(geometry::Computations comps, size_t remaining) {
  bool shadowed = IsShadowed(comps.over_point);

  utility::Color surface(0, 0, 0);
  for (auto &i : light_sources_) {
    // TODO: Properly implement support for multiple light sources
    surface += Lighting(comps.object->material_, comps.object->transform_,
                        *(light_sources_.front()), comps.point,
                        comps.eye_vector, comps.normal_vector, shadowed);
  }

  utility::Color reflected = ReflectedColor(comps, remaining);

  return surface + reflected;
}

utility::Color World::ColorAt(utility::Ray ray, size_t remaining) {
  std::vector<geometry::Intersection> intersections = Intersect(ray);
  auto hit = geometry::Hit(intersections);

  if (hit == std::nullopt) {
    return utility::Color(0, 0, 0);
  }

  geometry::Computations comps = hit->PrepareComputations(ray);
  return ShadeHit(comps, remaining);
}

bool World::IsShadowed(utility::Point point) {
  // TODO: Change the return value to a vector of booleans to account for each
  // light source in the scene
  utility::Vector v = light_sources_[0]->position_ - point;
  double distance = v.Magnitude();
  utility::Vector direction = v.Normalize();

  utility::Ray ray(point, direction);
  std::vector<geometry::Intersection> intersections = Intersect(ray);

  auto hit = geometry::Hit(intersections);
  if (hit != std::nullopt && hit.value().t_ < distance) {
    return true;
  }
  return false;
}

utility::Color World::ReflectedColor(geometry::Computations comps,
                                     size_t remaining) {
  if (remaining <= 0) {
    return utility::Color(0, 0, 0);
  }

  if (comps.object->material_.reflective_ == 0) {
    return utility::Color(0, 0, 0);
  }

  utility::Ray reflect_ray =
      utility::Ray(comps.over_point, comps.reflect_vector);
  utility::Color color = ColorAt(reflect_ray, remaining - 1);

  return color * comps.object->material_.reflective_;
}

World DefaultWorld() {
  PointLight light(utility::Point(-10, 10, -10), utility::Color(1, 1, 1));
  std::vector<std::shared_ptr<PointLight>> lights = {
      std::make_shared<PointLight>(light)};

  std::shared_ptr<geometry::Sphere> s1 = std::make_shared<geometry::Sphere>();
  s1->material_ = material::Material(utility::Color(0.8, 1.0, 0.6),
                                     std::nullopt, 0.1, 0.7, 0.2, 200);
  std::shared_ptr<geometry::Sphere> s2 = std::make_shared<geometry::Sphere>();
  s2->transform_ = utility::Scaling(0.5, 0.5, 0.5);
  std::vector<std::shared_ptr<geometry::Shape>> objects = {s1, s2};

  return World(lights, objects);
}

} // namespace scene
} // namespace raytracer
