//
//  Light.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Light_hpp
#define Light_hpp

#include "glm/glm.hpp"

using glm::vec3;

class Light {
public:
  vec3  position;
  vec3  color;
  
  Light(vec3 position_, vec3 color_)
    : position(position_), color(color_) {}
  
  Light() {
    position = vec3( 0, -0.5, -0.7 );
    color = 14.0f * vec3( 1, 1, 1 );
  }
  
  
};


#endif /* Light_hpp */
