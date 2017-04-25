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

class Cube : public Object
{
public:
  
  Cube(std::vector<Triangle> _triangles, Material _material)
    : Object( _triangles, _material) {}
  
  virtual Intersection intersect(Ray, int);
  
  vec3 Calculate_Intersection(Triangle, vec3, vec3);
  bool Intersects(vec3);


};

#endif /* Cube_hpp */
