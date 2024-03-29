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
  vec3 normal;
  float distance;
  int objIndex;
  int subIndex;
  bool didIntersect;

  
  Intersection()
  : position( vec3() ), distance( 5000.0f ), objIndex( -1 ), subIndex(-1), didIntersect( false ) {}
  
//  vec3 Calculate_Intersection(Triangle, vec3, vec3);
//  bool Intersects(vec3);

  
};

#endif /* Intersection_hpp */
