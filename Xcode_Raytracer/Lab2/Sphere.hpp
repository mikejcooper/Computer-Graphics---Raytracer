//
//  Sphere.hpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include <stdio.h>
#include "Object.hpp"
#include "Boundaries.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"


class Sphere : public Object
{
public:
  vec3 centre;
  float radius;
  Boundaries bounds;
  Material material;
  int id;
  

  
  Sphere(vec3 center_, float radius_,  Material _material)
  : centre(center_), radius(radius_), material(_material), id(1)
  {
    bounds.min = centre - vec3(radius, radius, radius);
    bounds.max = centre + vec3(radius, radius, radius);
  }
  
  Intersection intersect(Ray, int);
  Boundaries getBounds();
  vec3 getNormal(int, vec3);
  int getId();
  Material getMaterial();

private:
  vec3 getColor(vec3);
  
};

#endif /* Sphere_hpp */

