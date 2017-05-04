//
//  Ray.cpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Ray.hpp"

/**
 * Calculates the fractional direction for the ray to avoid doing it multiple times.
 */
void Ray::calcFractionalDirection() {
  fractionalDir.x = 1.0f / dir.x;
  fractionalDir.y = 1.0f / dir.y;
  fractionalDir.z = 1.0f / dir.z;
}
