#include <fstream>

#include "Camera.h"
#include "Canvas.h"
#include "CheckersPattern.h"
#include "GradientPattern.h"
#include "Matrix.h"
#include "Plane.h"
#include "RingPattern.h"
#include "Sphere.h"
#include "StripePattern.h"
#include "World.h"

int main(void) {
  raytracer::scene::World world;

  // Add a floor with a matte, striped texture
  auto floor = std::make_shared<raytracer::geometry::Plane>();
  floor->material_ = raytracer::material::Material();
  floor->material_.specular_ = 0;
  floor->material_.pattern_ =
      std::make_shared<raytracer::pattern::StripePattern>(
          raytracer::pattern::StripePattern(
              raytracer::utility::Color(0, 0, 0),
              raytracer::utility::Color(1, 1, 1)));
  world.objects_.push_back(floor);

  // Add a large unit sphere in the middle, translated upward slightly and
  // colored green
  auto middle = std::make_shared<raytracer::geometry::Sphere>();
  middle->transform_ = raytracer::utility::Translation(-0.5, 1, 0.5);
  middle->material_ = raytracer::material::Material();
  middle->material_.color_ = raytracer::utility::Color(0.204, 0.749, 0.286);
  middle->material_.diffuse_ = 0.7;
  middle->material_.specular_ = 0.3;
  middle->material_.pattern_ =
      std::make_shared<raytracer::pattern::GradientPattern>(
          raytracer::pattern::GradientPattern(
              raytracer::utility::Color(1, 0.298, 0.298),
              raytracer::utility::Color(0.204, 0.749, 0.286)));
  middle->material_.reflective_ = 0.8;
  middle->material_.pattern_.value()->transform_ =
      raytracer::utility::Translation(-0.5, 1, 0.5);
  world.objects_.push_back(middle);

  // Add a smaller blue sphere on the right
  auto right = std::make_shared<raytracer::geometry::Sphere>();
  right->transform_ = raytracer::utility::Identity()
                          .Scale(0.5, 0.5, 0.5)
                          .Translate(1.5, 0.5, -0.5);
  right->material_ = raytracer::material::Material();
  right->material_.color_ = raytracer::utility::Color(0, 0.60, 0.898);
  right->material_.diffuse_ = 0.7;
  right->material_.specular_ = 0.3;
  right->material_.pattern_ = std::make_shared<raytracer::pattern::RingPattern>(
      raytracer::pattern::RingPattern(
          raytracer::utility::Color(0.204, 0.749, 0.286),
          raytracer::utility::Color(0, 0.60, 0.898)));
  right->material_.pattern_.value()->transform_ =
      raytracer::utility::Identity().RotateX(M_PI_2).Scale(0.1, 0.1, 0.1);
  world.objects_.push_back(right);

  // Add an even smaller red sphere on the left
  auto left = std::make_shared<raytracer::geometry::Sphere>();
  left->transform_ = raytracer::utility::Identity()
                         .Scale(0.33, 0.33, 0.33)
                         .Translate(-1.5, 0.33, -0.75);
  left->material_ = raytracer::material::Material();
  left->material_.color_ = raytracer::utility::Color(1, 0.298, 0.298);
  left->material_.diffuse_ = 0.7;
  left->material_.specular_ = 0.3;
  left->material_.pattern_ =
      std::make_shared<raytracer::pattern::CheckersPattern>(
          raytracer::pattern::CheckersPattern(
              raytracer::utility::Color(0, 0.60, 0.898),
              raytracer::utility::Color(1, 0.298, 0.298)));
  left->material_.pattern_.value()->transform_ =
      raytracer::utility::Identity().Scale(0.25, 0.25, 1);
  world.objects_.push_back(left);

  // Add a white light source, shining from above and to the left
  world.light_sources_.push_back(std::make_shared<raytracer::scene::PointLight>(
      raytracer::scene::PointLight(raytracer::utility::Point(-10, 10, -10),
                                   raytracer::utility::Color(1, 1, 1))));

  // Configure the camera
  raytracer::scene::Camera camera(200, 100, M_PI / 3);
  camera.transform_ = raytracer::utility::ViewTransform(
      raytracer::utility::Point(0, 1.5, -5), raytracer::utility::Point(0, 1, 0),
      raytracer::utility::Vector(0, 1, 0));

  // Render the result to a canvas
  raytracer::Canvas canvas = camera.Render(world);

  // Write the canvas to a file
  std::ofstream out("scene.ppm");
  out << canvas.ToPpm();
  out.close();
}
