//
//  Intersection.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Intersection_hpp
#define Intersection_hpp

#include "glm/glm.hpp"
#include <iostream>

using namespace std;
using glm::vec3;



class Intersection {
public:
  vec3 position;
  float distance;
  std::pair <int, int> triangleIndex;
  bool didIntersect;

  
  Intersection()
  : position( vec3() ), distance( 0.0f ), triangleIndex( std::pair <int, int>() ), didIntersect( false ) {}
  
};

#endif /* Intersection_hpp */
