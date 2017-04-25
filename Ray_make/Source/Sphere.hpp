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

class Sphere : public Object
{
public:
  
  Sphere(std::vector<Triangle> _triangles, Material _material)
  : Object( _triangles, _material) {};
  
  Sphere()
  : Object() {};
  
  virtual Intersection intersect(Ray, int);
    
  vec3 Calculate_Intersection(Triangle, vec3, vec3);
  bool Intersects(vec3);
  
};

#endif /* Sphere_hpp */
