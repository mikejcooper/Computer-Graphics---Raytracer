//
//  Material.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Material_hpp
#define Material_hpp

#include "glm/glm.hpp"
using glm::vec3;

/* Material */
//typedef enum {
//  Diffuse,
//  Reflection,
//  ReflectionAndRefraction,
//  Phong,
//  Test,
//} Type;


class Material {
  
protected:
  float shininess;
  float reflectiveness;
  float refractiveIndex;
  vec3 color;
  int id;

public:
  Material (float _shininess, float _reflectiveness, float _refractiveIndex, int _id, vec3 _color)
    : shininess( _shininess ), reflectiveness( _reflectiveness ), refractiveIndex(_refractiveIndex), id(_id), color(_color) {}
  
//  Material() {};
  
  float getShininess();
  float getReflectivity();
  float getRefractiveIndex();
  float getId();
  vec3 getColor();
  
};


#endif /* Material_hpp */
