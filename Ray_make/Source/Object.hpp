//
//  Object.hpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Object_hpp
#define Object_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include <vector>
#include "Triangle.hpp"
#include "Material.hpp"
#include "Materials.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"


using namespace std;
using namespace glm;


class Object
{
public:
  
  std::vector<Triangle> triangles;
  Material material;

  Object(std::vector<Triangle> _triangles, Material _material)
  : triangles(_triangles), material(_material) {};
  
  Object() {};
  
  void addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
  {
    Triangle triangle = Triangle(v0,v1,v2,color);
    triangles.push_back(triangle);
  }
  
  virtual Intersection intersect(Ray, int) = 0;

};



#endif /* Object_hpp */
