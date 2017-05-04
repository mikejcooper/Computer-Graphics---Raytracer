//
//  Control.cpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Control.hpp"

void Control::Update(uint8_t* keystate){
  if (MOVEMENT){
    Camera(keystate);
    LightSource(keystate);
  }
  Features(keystate);
}

void Control::LightSource(uint8_t* keystate){
  vec3 translateX = vec3(0.1,0  ,0);
  vec3 translateY = vec3(0  ,0.1,0);
  vec3 translateZ = vec3(0  ,0  ,0.1);
  
  if( keystate[SDLK_w] ) {
    *lightPos += translateZ;
  }
  if( keystate[SDLK_s] ) {
    *lightPos -= translateZ;
  }
  if( keystate[SDLK_a] ) {
    *lightPos -= translateX;
  }
  if( keystate[SDLK_d] ) {
    *lightPos += translateX;
  }
  if( keystate[SDLK_q] ) {
    *lightPos += translateY;
  }
  if( keystate[SDLK_e] ) {
    *lightPos -= translateY;
  }
}

void Control::Camera(uint8_t* keystate){
  if( keystate[SDLK_UP] ) {
    vec3 translateForward = vec3(0,0,0.1);
    camera->position += translateForward * camera->rotation;
  }
  if( keystate[SDLK_DOWN] ) {
    vec3 translateForward = vec3(0,0,0.1);
    camera->position -= translateForward * camera->rotation;
  }
  if( keystate[SDLK_LEFT] ) {
    camera->rotation *= rotationLeft;
  }
  if( keystate[SDLK_RIGHT] ) {
    camera->rotation *= rotationRight;
  }
  if( keystate[SDLK_UP] && keystate[SDLK_RALT] ) {
    camera->rotation *= rotationUp;
  }
  if( keystate[SDLK_DOWN] && keystate[SDLK_RALT] ) {
    camera->rotation *= rotationDown;
  }
  if( keystate[SDLK_r] && keystate[SDLK_RALT] ) {
    camera->position = vec3(0.0,0.0,-3);
  }
}


void Control::Features(uint8_t* keystate){
  while(keystate[SDLK_LALT]) {
    SDL_PumpEvents(); // update key state array
    if(keystate[SDLK_s] && keystate[SDLK_EQUALS] ){
      SOFT_SHADOWS_SAMPLES_INC();
      while (keystate[SDLK_EQUALS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_s] && keystate[SDLK_MINUS] ){
      SOFT_SHADOWS_SAMPLES_DEC();
      while (keystate[SDLK_MINUS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_a] && keystate[SDLK_EQUALS] ){
      AA_SAMPLES_INC();
      while (keystate[SDLK_EQUALS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_a] && keystate[SDLK_MINUS] ){
      AA_SAMPLES_DEC();
      while (keystate[SDLK_MINUS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_d] && keystate[SDLK_EQUALS] ){
      DOF_SAMPLES_INC();
      while (keystate[SDLK_EQUALS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_d] && keystate[SDLK_MINUS] ){
      DOF_SAMPLES_DEC();
      while (keystate[SDLK_MINUS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_m]){
      MOVEMENT = (MOVEMENT) ? false : true;
      cout << "Movement " << MOVEMENT << endl;
      while (keystate[SDLK_m]) SDL_PumpEvents();
    }
    if(keystate[SDLK_e]){
      SHOW_EDGES = (SHOW_EDGES) ? false : true;
      cout << "Show Edges " << SHOW_EDGES << endl;
      while (keystate[SDLK_e]) SDL_PumpEvents();
    }
    if(keystate[SDLK_p]){
      TAKEPICTURE = (TAKEPICTURE) ? false : true;
      cout << "Take Picture " << TAKEPICTURE << endl;
      while (keystate[SDLK_p]) SDL_PumpEvents();
    }
    if(keystate[SDLK_k]){
      KDTREE = (KDTREE) ? false : true;
      cout << "KDTree " << KDTREE << endl;
      while (keystate[SDLK_k]) SDL_PumpEvents();
    }
    if(keystate[SDLK_g]){
      GLOBALILLUMINATION = (GLOBALILLUMINATION) ? false : true;
      cout << "Global Illumination " << GLOBALILLUMINATION << endl;
      while (keystate[SDLK_g]) SDL_PumpEvents();
    }
    if(keystate[SDLK_z]){
      PRINTLOCATION = (PRINTLOCATION) ? false : true;
      cout << "Printing info... " << PRINTLOCATION << endl;
      while (keystate[SDLK_z]) SDL_PumpEvents();
    }
    //    if(keystate[SDLK_d]){
    //      DOF = (DOF) ? false : true;
    //      cout << "Show Depth of Field " << DOF << endl;
    //      while (keystate[SDLK_d]) SDL_PumpEvents();
    //    }
    if(keystate[SDLK_q]){
      exit(0);
    }
  }
}



void Control::SOFT_SHADOWS_SAMPLES_INC(){
  if(SOFT_SHADOWS_SAMPLES + 6 < 32) {
    SOFT_SHADOWS_SAMPLES += 6;
  }
  else {
    SOFT_SHADOWS_SAMPLES = 31;
  }
  cout << "Soft Shadow sampling = " << SOFT_SHADOWS_SAMPLES << endl;
}

void Control::SOFT_SHADOWS_SAMPLES_DEC(){
  if(SOFT_SHADOWS_SAMPLES - 6 > 1) {
    SOFT_SHADOWS_SAMPLES -= 6;
  }
  else {
    SOFT_SHADOWS_SAMPLES = 1;
  }
  cout << "Anti-Aliasing sampling = " << SOFT_SHADOWS_SAMPLES << endl;
}

void Control::AA_SAMPLES_INC(){
  if(AA_SAMPLES + 1 < 30) {
    AA_SAMPLES += 1;
  }
  else {
    AA_SAMPLES = 30;
  }
  cout << "Anti-Aliasing sampling = " << AA_SAMPLES << endl;
}

void Control::AA_SAMPLES_DEC(){
  if(AA_SAMPLES - 1 > 1) {
    AA_SAMPLES -= 1;
  }
  else {
    AA_SAMPLES = 1;
  }
  cout << "Anti-Aliasing sampling = " << AA_SAMPLES << endl;
}

void Control::DOF_SAMPLES_INC(){
  if(DOF_VALUE + 1 < 30) {
    DOF_VALUE += 1;
  }
  else {
    DOF_VALUE = 30;
  }
  cout << "Depth of Field Value = " << DOF_VALUE << endl;
}

void Control::DOF_SAMPLES_DEC(){
  if(DOF_VALUE - 1 > 1) {
    DOF_VALUE -= 1;
  }
  else {
    DOF_VALUE = 1;
  }
  cout << "Depth of Field Value = " << DOF_VALUE << endl;
}
