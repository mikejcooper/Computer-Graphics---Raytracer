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
  
  virtual Intersection intersect(Ray, int) = 0;
  virtual Boundaries getBounds() = 0;
  virtual vec3 getNormal(int, vec3) = 0;
  virtual int getId() = 0;
  virtual Material getMaterial() = 0;
  
};



#endif /* Object_hpp */
