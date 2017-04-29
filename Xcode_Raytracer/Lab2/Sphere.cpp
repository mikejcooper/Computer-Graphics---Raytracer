//
//  Sphere.cpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Sphere.hpp"


#include <math.h>

Intersection Sphere::intersect(Ray ray, int i) {
  vec3 deltap = ray.start - centre;
  float a = glm::dot(ray.dir,ray.dir);
  float b = glm::dot(deltap, ray.dir) * 2;
  float c = glm::dot(deltap, deltap) - (radius * radius);
  
  float disc = b * b - 4 * a * c;
  if (disc < 0) {
    return Intersection(); // No intersection.
  }
  
  disc = sqrt(disc);
  
  float q;
  if (b < 0) {
    q = (-b - disc) * 0.5;
  } else {
    q = (-b + disc) * 0.5;
  }
  
  float r1 = q / a;
  float r2 = c / q;
  
  if (r1 > r2) {
    float tmp = r1;
    r1 = r2;
    r2 = tmp;
  }
  
  float distance = r1;
  if (distance < 0) {
    distance = r2;
  }
  
  if (distance < 0 || isnan(distance)) {
    return Intersection(); // No intersection.
  }
  
  vec3 point = ray.start + (ray.dir * distance);
  vec3 normal = glm::normalize(point - centre);
  
//  normal = material.modifyNormal(normal, point);
  
  
  // Normal needs to be flipped if this is a refractive ray.
  if (glm::dot(ray.dir,normal) > 0) {
    normal = normal * -1.0f;
  }
  
  Intersection intersection = Intersection();
  intersection.position = point;
  intersection.distance = distance;
  intersection.triangleIndex = make_pair(i, 0);
  intersection.didIntersect = true;
  intersection.normal = normal;

  
  return intersection;
//  return Intersection(ray, point, distance, normal, ray.material, material, this);
  
//  : position( vec3() ), distance( 5000.0f ), triangleIndex( std::pair <int, int>() ), didIntersect( false ) {}

  

}

Boundaries Sphere::getBounds() {
  return bounds;
}
