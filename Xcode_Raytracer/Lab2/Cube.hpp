//
//  Cube.hpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Cube_hpp
#define Cube_hpp

#include <stdio.h>
#include "Object.hpp"
#include "Triangle.hpp"

class Cube
{
public:
  
//  Shared
  Boundaries bounds;
  Material material;
  int id;

  Intersection intersect(Ray, int);
  Boundaries getBounds();
  vec3 getNormal(int, vec3);
  int getId();
  Material getMaterial();
  

  
//  Container / Cube
  std::vector<Triangle> triangles;
  vec3 Calculate_Intersection(Triangle, vec3, vec3);
  
  void Update_Bounds();
  bool Intersects(vec3);

  Cube(std::vector<Triangle> _triangles, Material _material)
    : triangles(_triangles), material(_material), id(0)
  {
    Update_Bounds();
  }
  
private:
  vec3 getNormal_Cube(int, vec3);
  void Update_Bounds_Cube();
  Intersection intersect_Cube(Ray, int);
  bool intersects_Cube(vec3);
  vec3 Calculate_Intersection_Cube(Triangle, vec3, vec3);

  



  //   Sphere

public:
  vec3 centre;
  float radius;
  
  Cube(vec3 center_, float radius_,  Material _material)
  : centre(center_), radius(radius_), material(_material), id(1)
  {
    bounds.min = centre - vec3(radius, radius, radius);
    bounds.max = centre + vec3(radius, radius, radius);
  }
  
private:
  Intersection intersect_Sphere(Ray ray, int i);
  vec3 getNormal_Sphere(int subIndex, vec3 intersectPosition);
  void Update_Bounds_Sphere();

  
  
  
};

#endif /* Cube_hpp */
