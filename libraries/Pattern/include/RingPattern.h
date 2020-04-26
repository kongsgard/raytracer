#pragma once

#include "Color.h"
#include "Pattern.h"
#include "Point.h"

namespace raytracer {
namespace pattern {

class RingPattern : public Pattern {
public:
  RingPattern(utility::Color a, utility::Color b) : Pattern(), a_(a), b_(b) {}

  utility::Color PatternAt(const utility::Point &point);

  utility::Color a_;
  utility::Color b_;
};

} // namespace pattern
} // namespace raytracer
