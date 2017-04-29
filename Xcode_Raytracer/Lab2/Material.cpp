//
//  Material.cpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Material.hpp"


float Material::getShininess() {
  return shininess;
}

float Material::getReflectivity() {
  return reflectiveness;
}

float Material::getRefractiveIndex() {
  return refractiveIndex;
}

float Material::getId() {
  return id;
}

vec3 Material::getColor() {
  return color;
}
