//
//  Cube.cpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Cube.hpp"

Intersection Cube::intersect(Ray ray, int i) {
  Intersection intersection = Intersection();
  
  for (int j = 0; j < triangles.size(); ++j) {
    vec3 x =  Calculate_Intersection(triangles[j], ray.start, ray.dir);
    if(i != ray.objectIndex && Intersects(x))  {
      // If the current intersection is closer than previous, update
      if(intersection.distance > x.x) {
        intersection.distance = x.x;
        intersection.position = ray.start + x.x * ray.dir;
        intersection.objIndex = i;
        intersection.didIntersect = true;
        // Unit vector perpendicular to plane.
        intersection.normal = glm::normalize(triangles[j].normal);
        intersection.objIndexPrevious = ray.objectIndex;
      }
    }
  }
  
  return intersection;
}




vec3 Cube::Calculate_Intersection(Triangle triangle, vec3 start, vec3 dir) {
  // vi represents the vertices of the triangle
  vec3 v0 = triangle.v0;
  vec3 v1 = triangle.v1;
  vec3 v2 = triangle.v2;
  
  vec3 e1 = v1 - v0;    // Vector parallel to edge of the triangle between v0 and v1
  vec3 e2 = v2 - v0;    // Vector parallel to edge of the triangle between v0 and v2
  vec3 b = start - v0;  // Vector parallel to edge between v0 and camara position
  
  
  // Cramer's rule: faster than   // mat3 A( -dir, e1, e2 ); return inverse( A ) * b;
  vec3 cross_e1e2 = cross(e1,e2);
  vec3 cross_be2 = cross(b,e2);
  vec3 cross_e1b = cross(e1,b);
  
  float dot_e1e2b = dot(cross_e1e2, b);
  
  float dot_e1e2d = dot(cross_e1e2, -dir);
  float dot_be2d =  dot(cross_be2, -dir);
  float dot_e1bd =  dot(cross_e1b, -dir);
  
  // Point of intersection: x = (t, u, v), from v0 + ue1 + ve2 = s + td
  vec3 x = vec3(dot_e1e2b / dot_e1e2d, dot_be2d / dot_e1e2d, dot_e1bd / dot_e1e2d);
  
  
  return x;
}

bool Cube::Intersects(vec3 x){
  // Check if largest float value can hold intersection distance
  float maxDist = std::numeric_limits<float>::max();
  // Check x statisfy rules for intersection
  return (x.x <= maxDist) && (0 <= x.y && 0 <= x.z && 0 <= x.x && (x.y + x.z) <= 1);
}

