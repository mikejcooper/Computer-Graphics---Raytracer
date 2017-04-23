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
  Glass ()
  : Material(50.0f, 0.0f, 2.0f) {}
};

class Phong : public Material {
  
public:
  //  float _shininess, float _reflectiveness, float _refractiveIndex
  Phong ()
  : Material(100.0f, 0.9f, 0.0f) {}
};

class Diffuse : public Material {
  
public:
  //  float _shininess, float _reflectiveness, float _refractiveIndex
  Diffuse ()
  : Material(0.0f, 0.0f, 0.0f) {}
};



#endif /* Materials_hpp */
