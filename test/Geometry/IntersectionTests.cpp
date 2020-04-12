#include "Intersection.h"

#include <gtest/gtest.h>

#include "Ray.h"
#include "Sphere.h"

using namespace raytracer;

TEST(IntersectionTests, IntersectionEncapsulatesTAndObject) {
  geometry::Sphere s;
  geometry::Intersection i(3.5, s);
  ASSERT_DOUBLE_EQ(3.5, i.t_);
  EXPECT_TRUE(s == i.object_);
}

TEST(IntersectionTests, AggregatingIntersections) {
  geometry::Sphere s;
  geometry::Intersection i1(1, s);
  geometry::Intersection i2(2, s);
  std::vector<geometry::Intersection> xs = geometry::Intersections({i1, i2});
  ASSERT_EQ(2, xs.size());
  ASSERT_DOUBLE_EQ(1, xs[0].t_);
  ASSERT_DOUBLE_EQ(2, xs[1].t_);
}

TEST(IntersectionTests, IntersectSetsTheObjectOnTheIntersection) {
  utility::Ray r(utility::Point(0, 0, -5), utility::Vector(0, 0, 1));
  geometry::Sphere s;
  std::vector<geometry::Intersection> xs = geometry::Intersect(s, r);
  ASSERT_EQ(2, xs.size());
  EXPECT_TRUE(s == xs[0].object_);
  EXPECT_TRUE(s == xs[1].object_);
}

TEST(IntersectionTests, TheHitWhenAllIntersectionsHavePositiveT) {
  geometry::Sphere s;
  geometry::Intersection i1(1, s);
  geometry::Intersection i2(2, s);
  std::vector<geometry::Intersection> xs = geometry::Intersections({i1, i2});
  auto i = geometry::Hit(xs);
  EXPECT_TRUE(i1 == i.value());
}

TEST(IntersectionTests, TheHitWhenSomeIntersectionsHaveNegativeT) {
  geometry::Sphere s;
  geometry::Intersection i1(-1, s);
  geometry::Intersection i2(1, s);
  std::vector<geometry::Intersection> xs = geometry::Intersections({i1, i2});
  auto i = geometry::Hit(xs);
  EXPECT_TRUE(i2 == i.value());
}

TEST(IntersectionTests, TheHitWhenAllIntersectionsHaveNegativeT) {
  geometry::Sphere s;
  geometry::Intersection i1(-2, s);
  geometry::Intersection i2(-1, s);
  std::vector<geometry::Intersection> xs = geometry::Intersections({i1, i2});
  auto i = geometry::Hit(xs);
  EXPECT_TRUE(std::nullopt == i);
}

TEST(IntersectionTests, TheHitIsAlwaysTheLowestNonnegativeIntersection) {
  geometry::Sphere s;
  geometry::Intersection i1(5, s);
  geometry::Intersection i2(7, s);
  geometry::Intersection i3(-3, s);
  geometry::Intersection i4(2, s);
  std::vector<geometry::Intersection> xs =
      geometry::Intersections({i1, i2, i3, i4});
  auto i = geometry::Hit(xs);
  EXPECT_TRUE(i4 == i.value());
}

TEST(IntersectionTests, PrecomputeStateOfIntersection) {
  utility::Ray r(utility::Point(0, 0, -5), utility::Vector(0, 0, 1));
  geometry::Sphere shape;
  geometry::Intersection i(4, shape);
  geometry::Computations comps = i.PrepareComputations(r);
  EXPECT_TRUE(i.t_ == comps.t);
  EXPECT_TRUE(i.object_ == comps.object);
  EXPECT_TRUE(utility::Point(0, 0, -1) == comps.point);
  EXPECT_TRUE(utility::Vector(0, 0, -1) == comps.eye_vector);
  EXPECT_TRUE(utility::Vector(0, 0, -1) == comps.normal_vector);
}

TEST(IntersectionTests, HitWhenIntersectionOccursOnTheOutside) {
  utility::Ray r(utility::Point(0, 0, -5), utility::Vector(0, 0, 1));
  geometry::Sphere shape;
  geometry::Intersection i(4, shape);
  geometry::Computations comps = i.PrepareComputations(r);
  EXPECT_FALSE(comps.inside);
}

TEST(IntersectionTests, HitWhenIntersectionOccursOnTheInside) {
  utility::Ray r(utility::Point(0, 0, 0), utility::Vector(0, 0, 1));
  geometry::Sphere shape;
  geometry::Intersection i(1, shape);
  geometry::Computations comps = i.PrepareComputations(r);
  EXPECT_TRUE(utility::Point(0, 0, 1) == comps.point);
  EXPECT_TRUE(utility::Vector(0, 0, -1) == comps.eye_vector);
  EXPECT_TRUE(comps.inside);

  // The normal would have been (0, 0, 1), but is inverted!
  EXPECT_TRUE(utility::Vector(0, 0, -1) == comps.normal_vector);
}

TEST(IntersectionTests, TheHitShouldOffsetThePoint) {
  utility::Ray ray(utility::Point(0, 0, -5), utility::Vector(0, 0, 1));
  geometry::Sphere shape;
  shape.transform_ = utility::Translation(0, 0, 1);
  geometry::Intersection intersection(5, shape);
  geometry::Computations comps = intersection.PrepareComputations(ray);
  ASSERT_LT(comps.over_point.z(), -EPSILON / 2);
  ASSERT_GT(comps.point.z(), comps.over_point.z());
}
