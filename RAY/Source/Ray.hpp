//
//  Ray.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Ray_hpp
#define Ray_hpp

#include "glm/glm.hpp"

using glm::vec3;

class Ray {

public:
  vec3 start;
  vec3 dir;
  int objectIndex;
  
  Ray()
    : start ( vec3() ), dir( vec3() ), objectIndex( -1 ) {}
  
  Ray(vec3 _start, vec3 _dir, int _objectIndex)
    : start ( _start ), dir( _dir ), objectIndex( _objectIndex )  {}
};

#endif /* Ray_hpp */
