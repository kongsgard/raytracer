#include <fstream>

#include "Camera.h"
#include "Canvas.h"
#include "Matrix.h"
#include "Sphere.h"
#include "World.h"

/* Plots position of the hours on the analog clock face */
int main(void) {
  raytracer::scene::World world;

  // Add a floor as an extremely flattened sphere with a matte texture
  raytracer::geometry::Sphere floor;
  floor.transform_ = raytracer::utility::Scaling(10, 0.01, 10);
  floor.material_ = raytracer::material::Material();
  floor.material_.color_ = raytracer::utility::Color(1, 0.9, 0.9);
  floor.material_.specular_ = 0;
  world.objects_.push_back(
      std::make_shared<raytracer::geometry::Sphere>(floor));

  // Add a wall on the left which as the same scale and color as the floor, but
  // which is also rotated and translated into place
  raytracer::geometry::Sphere left_wall;
  left_wall.transform_ = raytracer::utility::Identity()
                             .Scale(10, 0.01, 10)
                             .RotateX(M_PI_2)
                             .RotateY(-M_PI_4)
                             .Translate(0, 0, 5);
  left_wall.material_ = floor.material_;
  world.objects_.push_back(
      std::make_shared<raytracer::geometry::Sphere>(left_wall));

  // Add a wall on the right which is identical to the left wall, but which
  // is rotated the opposite direction in y
  raytracer::geometry::Sphere right_wall;
  right_wall.transform_ = raytracer::utility::Identity()
                              .Scale(10, 0.01, 10)
                              .RotateX(M_PI_2)
                              .RotateY(M_PI_4)
                              .Translate(0, 0, 5);
  right_wall.material_ = floor.material_;
  world.objects_.push_back(
      std::make_shared<raytracer::geometry::Sphere>(right_wall));

  // Add a large unit sphere in the middle, translated upward slightly and
  // colored green
  raytracer::geometry::Sphere middle;
  middle.transform_ = raytracer::utility::Translation(-0.5, 1, 0.5);
  middle.material_ = raytracer::material::Material();
  middle.material_.color_ = raytracer::utility::Color(0.204, 0.749, 0.286);
  middle.material_.diffuse_ = 0.7;
  middle.material_.specular_ = 0.3;
  world.objects_.push_back(
      std::make_shared<raytracer::geometry::Sphere>(middle));

  // Add a smaller blue sphere on the right
  raytracer::geometry::Sphere right;
  right.transform_ = raytracer::utility::Identity()
                         .Scale(0.5, 0.5, 0.5)
                         .Translate(1.5, 0.5, -0.5);
  right.material_ = raytracer::material::Material();
  right.material_.color_ = raytracer::utility::Color(0, 0.60, 0.898);
  right.material_.diffuse_ = 0.7;
  right.material_.specular_ = 0.3;
  world.objects_.push_back(
      std::make_shared<raytracer::geometry::Sphere>(right));

  // Add an even smaller red sphere on the left
  raytracer::geometry::Sphere left;
  left.transform_ = raytracer::utility::Identity()
                        .Scale(0.33, 0.33, 0.33)
                        .Translate(-1.5, 0.33, -0.75);
  left.material_ = raytracer::material::Material();
  left.material_.color_ = raytracer::utility::Color(1, 0.298, 0.298);
  left.material_.diffuse_ = 0.7;
  left.material_.specular_ = 0.3;
  world.objects_.push_back(std::make_shared<raytracer::geometry::Sphere>(left));

  // Add a white light source, shining from above and to the left
  world.light_sources_.push_back(std::make_shared<raytracer::scene::PointLight>(
      raytracer::scene::PointLight(raytracer::utility::Point(-10, 10, -10),
                                   raytracer::utility::Color(1, 1, 1))));

  // Configure the camera
  raytracer::scene::Camera camera(1280, 720, M_PI / 3);
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
