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
#include "Material.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"
#include "Boundaries.hpp"


using namespace std;
using namespace glm;


class Object
{
public:
  
  Material material;
  int id;

  Object(Material _material, int _id)
  : material(_material), id(_id) {};
  
//  Object() {};
  
  virtual Intersection intersect(Ray, int) = 0;
  virtual Boundaries getBounds() = 0;
  virtual vec3 getNormal(int, vec3) = 0;

  
  int getId();

};



#endif /* Object_hpp */
