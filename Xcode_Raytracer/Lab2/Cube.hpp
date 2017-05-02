//
//  Cube.hpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Cube_hpp
#define Cube_hpp

#include <stdio.h>
#include "Object.hpp"
#include "Triangle.hpp"

class Cube : public Object
{
public:
  std::vector<Triangle> triangles;
  Boundaries bounds;

  Cube(std::vector<Triangle> _triangles, Material _material)
    : triangles(_triangles), Object(_material, 0)
  {
    Update_Bounds();
  }
  
  virtual Intersection intersect(Ray, int);
  virtual Boundaries getBounds();
  
  vec3 Calculate_Intersection(Triangle, vec3, vec3);
  void Update_Bounds();
  bool Intersects(vec3);
};

#endif /* Cube_hpp */
