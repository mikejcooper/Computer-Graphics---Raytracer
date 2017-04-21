//
//  Camera.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "glm/glm.hpp"

using glm::vec3;
using glm::mat3;

class Camera {
  
public:
  vec3  position;
  mat3  rotation;
  
  Camera()
  : position( NULL ), rotation( NULL ) {}
  
  Camera(vec3 _cameraPos, mat3 _cameraRot)
  : position( _cameraPos ), rotation( _cameraRot ) {}
  
  
  
};

#endif /* Camera_hpp */
