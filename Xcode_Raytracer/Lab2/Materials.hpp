//
//  Materials.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Materials_hpp
#define Materials_hpp

#include <stdio.h>
#include "Material.hpp"


class Glass : public Material {

public:
//  float _shininess, float _reflectiveness, float _refractiveIndex
  Glass (vec3 color)
  : Material(5.0f, 0.5f, 1.5f, 2, color) {}
};

class Phong : public Material {
  
public:
  //  float _shininess, float _reflectiveness, float _refractiveIndex
  Phong (vec3 color)
  : Material(0.1f, 0.6f, 1.0f, 0, color) {}
//  : Material(100.0f, 0.9f, 0.0f, 1, color) {}
};

class Diffuse : public Material {
  
public:
  //  float _shininess, float _reflectiveness, float _refractiveIndex
  Diffuse (vec3 color)
  : Material(0.0f, 0.0f, 1.0f, 0, color) {}
};



#endif /* Materials_hpp */
