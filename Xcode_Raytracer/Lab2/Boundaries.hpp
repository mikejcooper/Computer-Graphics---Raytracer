//
//  Boundaries.hpp
//  Raytracer
//
//  Created by Mike Cooper on 29/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Boundaries_hpp
#define Boundaries_hpp

#include <stdio.h>

#include "Ray.hpp"

class Boundaries {
public:
  vec3 min, max;
  
  Boundaries() : min(vec3(0, 0, 0)), max(vec3(0, 0, 0)) {}
  
  float splitValue(char axis);
  bool intersect(const Ray&, float* dist);
};



#endif /* Boundaries_hpp */
