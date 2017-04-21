//
//  Material.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Material_hpp
#define Material_hpp


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

public:
  Material (float _shininess, float _reflectiveness, float _refractiveIndex)
    : shininess( _shininess ), reflectiveness( _reflectiveness ), refractiveIndex(_refractiveIndex) {}
  
  virtual float getShininess();
  virtual float getReflectivity();
  virtual float getRefractiveIndex();
  
};


#endif /* Material_hpp */
