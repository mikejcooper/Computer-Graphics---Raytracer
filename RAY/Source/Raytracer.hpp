//
//  Ray.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Raytracer_hpp
#define Raytracer_hpp

#include <stdio.h>
#include "glm/glm.hpp"

#include "Intersection.hpp"
#include "Color.hpp"
#include "Ray.hpp"


using namespace std;
using glm::vec3;



class RayTracer {
public:
  
  
private:
  
  Color castRayForPixel(int, int);
  Color castRay(const Ray&);
//  bool isInShadow(const Ray&, double);
//  Intersection getClosestIntersection(const Ray&);
  Color performLighting(const Intersection&);
  Color getAmbientLighting(const Intersection&, const Color&);
  Color getDiffuseAndSpecularLighting(const Intersection&, const Color&);
//  Color getSpecularLighting(const Intersection&, Light*);
  Color getReflectiveRefractiveLighting(const Intersection&);
  double getReflectance(const vec3&, const vec3&, double, double);
  vec3 refractVector(const vec3&, const vec3&, double, double);
  vec3 reflectVector(vec3, vec3);
};

#endif /* Ray_hpp */
