#include "Sphere.hpp"

#include <cmath>

namespace raytracer {
namespace geometry {

double Sphere::Discriminant(utility::Ray ray) {
  utility::Vector sphere_to_ray = ray.origin_ - origin_;

  double a = ray.direction_.Dot(ray.direction_);
  double b = 2.0 * ray.direction_.Dot(sphere_to_ray);
  double c = sphere_to_ray.Dot(sphere_to_ray) - 1.0;

  return b * b - 4 * a * c;
}

std::vector<Intersection> Sphere::LocalIntersect(utility::Ray &ray) {
  utility::Vector sphere_to_ray = ray.origin_ - origin_;

  double a = ray.direction_.Dot(ray.direction_);
  double b = 2.0 * ray.direction_.Dot(sphere_to_ray);
  double c = sphere_to_ray.Dot(sphere_to_ray) - 1.0;
  double discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return std::vector<Intersection>{};
  }

  double t1 = (-b - sqrt(discriminant)) / (2 * a);
  double t2 = (-b + sqrt(discriminant)) / (2 * a);
  return std::vector<Intersection>{
      Intersection(t1, std::make_shared<Sphere>(*this)),
      Intersection(t2, std::make_shared<Sphere>(*this))};
}

utility::Vector Sphere::LocalNormalAt(utility::Point &point) {
  return point - utility::Point(0, 0, 0);
}

Bounds Sphere::ComputeBounds() {
  return Bounds(utility::Point(-1, -1, -1), utility::Point(1, 1, 1));
}

Sphere GlassSphere() {
  Sphere sphere;
  sphere.transform_ = utility::Identity();
  sphere.material_->transparency_ = 1.0;
  sphere.material_->refractive_index_ = 1.5;
  return sphere;
}

} // namespace geometry
} // namespace raytracer
