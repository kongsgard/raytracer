#include "Camera.h"

#include <gtest/gtest.h>

using namespace raytracer;

TEST(CameraTests, ConstructCamera) {
  size_t hsize = 160;
  size_t vsize = 120;
  double field_of_view = M_PI_2;
  scene::Camera c(hsize, vsize, field_of_view);
  ASSERT_EQ(160, c.hsize_);
  ASSERT_EQ(120, c.vsize_);
  ASSERT_DOUBLE_EQ(M_PI_2, c.field_of_view_);
  EXPECT_TRUE(utility::Identity() == c.transform_);
}

TEST(CameraTests, PixelSizeForHorizontalCanvas) {
  scene::Camera c(200, 125, M_PI_2);
  ASSERT_DOUBLE_EQ(0.01, c.pixel_size_);
}

TEST(CameraTests, PixelSizeForVerticalCanvas) {
  scene::Camera c(125, 200, M_PI_2);
  ASSERT_DOUBLE_EQ(0.01, c.pixel_size_);
}

TEST(CameraTests, ConstructRayThroughCenterOfCanvas) {
  scene::Camera c(201, 101, M_PI_2);
  utility::Ray r = c.RayForPixel(100, 50);
  EXPECT_TRUE(utility::Point(0, 0, 0) == r.origin_);
  EXPECT_TRUE(utility::Vector(0, 0, -1) == r.direction_);
}

TEST(CameraTests, ConstructRayThroughCornerOfCanvas) {
  scene::Camera c(201, 101, M_PI_2);
  utility::Ray r = c.RayForPixel(0, 0);
  EXPECT_TRUE(utility::Point(0, 0, 0) == r.origin_);
  EXPECT_TRUE(utility::Vector(0.66518642611945078, 0.33259321305972539,
                              -0.66851235825004807) == r.direction_);
}

TEST(CameraTests, ConstructRayWhenCameraIsTransformed) {
  scene::Camera c(201, 101, M_PI_2);
  c.transform_ = utility::RotationY(M_PI_4) * utility::Translation(0, -2, 5);
  utility::Ray r = c.RayForPixel(100, 50);
  EXPECT_TRUE(utility::Point(0, 2, -5) == r.origin_);
  EXPECT_TRUE(utility::Vector(sqrt(2) / 2, 0, -sqrt(2) / 2) == r.direction_);
}

TEST(CameraTests, RenderWorldWithCamera) {
  scene::World w = scene::DefaultWorld();
  scene::Camera c(11, 11, M_PI_2);
  utility::Point from(0, 0, -5);
  utility::Point to(0, 0, 0);
  utility::Vector up(0, 1, 0);
  c.transform_ = utility::ViewTransform(from, to, up);
  Canvas image = c.Render(w);
  EXPECT_TRUE(utility::Color(0.38066119308103435, 0.47582649135129296,
                             0.28549589481077575) == image.GetPixel(5, 5));
}
