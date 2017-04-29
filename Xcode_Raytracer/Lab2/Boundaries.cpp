//
//  Boundaries.cpp
//  Raytracer
//
//  Created by Mike Cooper on 29/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Boundaries.hpp"


// Returns the center value for the given axis.
float Boundaries::splitValue(char axis) {
  switch(axis) {
    case 'x': return (min.x + max.x) / 2;
    case 'y': return (min.y + max.y) / 2;
    case 'z': return (min.z + max.z) / 2;
    default: return 0.0f;
  }
}

/**
 * Ray axis aligned bounding box intersection.
 */
bool Boundaries::intersect(const Ray& ray, float* dist) {
  // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
  // r.org is origin of ray
  float t1 = (min.x - ray.start.x) * ray.fractionalDir.x;
  float t2 = (max.x - ray.start.x) * ray.fractionalDir.x;
  float t3 = (min.y - ray.start.y) * ray.fractionalDir.y;
  float t4 = (max.y - ray.start.y) * ray.fractionalDir.y;
  float t5 = (min.z - ray.start.z) * ray.fractionalDir.z;
  float t6 = (max.z - ray.start.z) * ray.fractionalDir.z;
  
  float tmin = std::fmax(std::fmax(std::fmin(t1, t2), std::fmin(t3, t4)), std::fmin(t5, t6));
  float tmax = std::fmin(std::fmin(std::fmax(t1, t2), std::fmax(t3, t4)), std::fmax(t5, t6));
  
  // If tmax < 0, ray is intersecting AABB, but whole AABB is behind us.
  if (tmax < 0) {
    return false;
  }
  
  // If tmin > tmax, ray doesn't intersect AABB.
  if (tmin > tmax) {
    return false;
  }
  
  return true;
}



