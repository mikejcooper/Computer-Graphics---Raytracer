//
//  Control.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Control_hpp
#define Control_hpp

#include <stdint.h>
#include "glm/glm.hpp"
#include <stdint.h>
#include <SDL.h>
#include "Camera.hpp"
#include <math.h>
#include <iostream>




using glm::vec3;
using glm::mat3;
using namespace std;


class Control {
  
public:
  vec3* lightPos;
  Camera* camera;
  
  float TURN_ANGLE;
  mat3  rotationUp;
  mat3  rotationDown;
  mat3  rotationLeft;
  mat3  rotationRight;
  
  int   SOFT_SHADOWS_SAMPLES;
  int   AA_SAMPLES;
  int   DOF_VALUE;
  bool  MOVEMENT;
  bool  SHOW_EDGES;
  bool  DOF;
  bool  TAKEPICTURE;
  
  Control(){};
  
  Control(vec3* _lightPos, Camera* _camera){
    lightPos = _lightPos;
    camera = _camera;
    
    TURN_ANGLE = (M_PI / 180) * 6;
    rotationUp = mat3(1,0,0,0,cos(TURN_ANGLE),-sin(TURN_ANGLE),0,sin(TURN_ANGLE),cos(TURN_ANGLE));
    rotationDown = mat3(1,0,0,0,cos(-TURN_ANGLE),-sin(-TURN_ANGLE),0,sin(-TURN_ANGLE),cos(-TURN_ANGLE));
    rotationLeft = mat3(cos(TURN_ANGLE),0,-sin(TURN_ANGLE),0,1,0,sin(TURN_ANGLE),0,cos(TURN_ANGLE));
    rotationRight = mat3(cos(-TURN_ANGLE),0,-sin(-TURN_ANGLE),0,1,0,sin(-TURN_ANGLE),0,cos(-TURN_ANGLE));
    
    SOFT_SHADOWS_SAMPLES = 1;
    AA_SAMPLES = 1;
    DOF_VALUE = 1;
    MOVEMENT = true;
    SHOW_EDGES = false;
    DOF = false;
    TAKEPICTURE = false;
  
  }
  
  void Update(uint8_t*);
  void LightSource(uint8_t*);
  void Camera(uint8_t*);
  void Features(uint8_t*);
  

private:
  void SOFT_SHADOWS_SAMPLES_INC();
  void SOFT_SHADOWS_SAMPLES_DEC();
  void AA_SAMPLES_INC();
  void AA_SAMPLES_DEC();
  void DOF_SAMPLES_INC();
  void DOF_SAMPLES_DEC();

};



#endif /* Control_hpp */
