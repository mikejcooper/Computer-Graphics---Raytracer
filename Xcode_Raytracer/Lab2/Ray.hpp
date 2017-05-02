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
  vec3 fractionalDir;
  int objectIndex;
  bool isPrimary;
  
  Ray()
    : start ( vec3() ), dir( vec3() ), objectIndex( -1 ), isPrimary(false) {}
  
  Ray(vec3 _start, vec3 _dir, int _objectIndex, bool _isPrimary)
    : start ( _start ), dir( _dir ), objectIndex( _objectIndex ), isPrimary(_isPrimary)
  {
    calcFractionalDirection();
  }
  
private:
  void calcFractionalDirection();

};

#endif /* Ray_hpp */
