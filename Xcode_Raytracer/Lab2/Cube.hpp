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
  
  Cube(std::vector<Triangle> _triangles, Material _material)
    : triangles(_triangles), Object(_material, 0) {}
  
  virtual Intersection intersect(Ray, int);
  
  vec3 Calculate_Intersection(Triangle, vec3, vec3);
  bool Intersects(vec3);

  void addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
  {
    Triangle triangle = Triangle(v0,v1,v2,color);
    triangles.push_back(triangle);
  }

};

#endif /* Cube_hpp */
