#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#define _USE_MATH_DEFINES
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdparty/stb/stb_image_write.h"

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

void write_canvas_to_image(std::filesystem::path &file_path,
                           const raytracer::Canvas &canvas) {
  size_t channels = 3;

  auto get_image_data = [](size_t width, size_t height, size_t channels,
                           const raytracer::Canvas &canvas) {
    std::vector<uint8_t> image_data(canvas.width_ * canvas.height_ * channels);
    size_t i = 0;
    for (size_t col = 0; col < canvas.height_; ++col) {
      for (size_t row = 0; row < canvas.width_; ++row) {
        for (size_t k = 0; k < channels; ++k) {
          image_data[i++] = (uint8_t)std::clamp(
              static_cast<int>(canvas.framebuffer_[row][col][k] * 256), 0, 255);
        }
      }
    }

    return image_data;
  };

  if (file_path.extension() == ".ppm") {
    // Handle special case ppm implemented in the Canvas class
    std::ofstream out(file_path.string().c_str());
    out << canvas.ToPpm();
    out.close();
  } else if (file_path.extension() == ".bmp") {
    stbi_write_bmp(
        file_path.string().c_str(), canvas.width_, canvas.height_, channels,
        get_image_data(canvas.width_, canvas.height_, channels, canvas).data());
  } else if (file_path.extension() == ".jpg") {
    stbi_write_jpg(
        file_path.string().c_str(), canvas.width_, canvas.height_, channels,
        get_image_data(canvas.width_, canvas.height_, channels, canvas).data(),
        100);
  } else if (file_path.extension() == ".png") {
    stbi_write_png(
        file_path.string().c_str(), canvas.width_, canvas.height_, channels,
        get_image_data(canvas.width_, canvas.height_, channels, canvas).data(),
        canvas.width_ * channels);
  } else {
    throw std::invalid_argument("Invalid image file format provided.");
  }
}

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
  write_canvas_to_image(std::filesystem::current_path() / "scene.bmp", canvas);
}
