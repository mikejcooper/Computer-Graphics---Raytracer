//
//  Cube.cpp
//  Lab2
//
//  Created by Mike Cooper on 23/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Cube.hpp"

Intersection Cube::intersect(Ray ray, int i) {
  if(id == 0){
    return intersect_Cube(ray, i);
  } else{
    return intersect_Sphere(ray, i);
  }
}

bool Cube::Intersects(vec3 x){
  // Check if largest float value can hold intersection distance
  float maxDist = std::numeric_limits<float>::max();
  // Check x statisfy rules for intersection
  return (x.x <= maxDist) && (0 <= x.y && 0 <= x.z && 0 <= x.x && (x.y + x.z) <= 1);
}

Boundaries Cube::getBounds() {
  return bounds;
}

vec3 Cube::getNormal(int subIndex, vec3 intersectPosition) {
  if (id == 0){
    getNormal_Cube(subIndex, intersectPosition);
  } else {
    getNormal_Sphere(subIndex, intersectPosition);
  }
  
  return triangles[subIndex].normal;
}

Material Cube::getMaterial(){
  return material;
}

int Cube::getId(){
  return id;
}


void Cube::Update_Bounds()
{
  if (id == 0){
    triangles[0].Update_Bounds();
    bounds = triangles[0].bounds;
    for (vector<Triangle>::iterator itr = triangles.begin(); itr < triangles.end(); itr++)
    {
      (itr)->Update_Bounds();
      Boundaries b = (itr)->bounds;
      
      cout << bounds.max.x <<endl;
      
      bounds.min = vec3(std::min(b.min.x, bounds.min.x),
                        std::min(b.min.y, bounds.min.y),
                        std::min(b.min.z, bounds.min.z));
      
      bounds.max = vec3(std::max(b.max.x, bounds.max.x),
                        std::max(b.max.y, bounds.max.y),
                        std::max(b.max.z, bounds.max.z));
    }
  }
}





// Container / Cube

Intersection Cube::intersect_Cube(Ray ray, int i) {
  Intersection intersection = Intersection();
  
  for (int j = 0; j < triangles.size(); ++j) {
    vec3 x =  Calculate_Intersection_Cube(triangles[j], ray.start, ray.dir);
    if(i != ray.objectIndex && intersects_Cube(x))  {
      // If the current intersection is closer than previous, update
      if(intersection.distance > x.x) {
        intersection.distance = x.x;
        intersection.position = ray.start + x.x * ray.dir;
        intersection.objIndex = i;
        intersection.didIntersect = true;
        // Unit vector perpendicular to plane.
        intersection.normal = glm::normalize(triangles[j].normal);
        intersection.subIndex = j;
      }
    }
  }
  
  return intersection;
}

vec3 Cube::Calculate_Intersection_Cube(Triangle triangle, vec3 start, vec3 dir) {
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

bool Cube::intersects_Cube(vec3 x){
  // Check if largest float value can hold intersection distance
  float maxDist = std::numeric_limits<float>::max();
  // Check x statisfy rules for intersection
  return (x.x <= maxDist) && (0 <= x.y && 0 <= x.z && 0 <= x.x && (x.y + x.z) <= 1);
}



void Cube::Update_Bounds_Cube()
{
  triangles[0].Update_Bounds();
  bounds = triangles[0].bounds;
  for (vector<Triangle>::iterator itr = triangles.begin(); itr < triangles.end(); itr++)
  {
    (itr)->Update_Bounds();
    Boundaries b = (itr)->bounds;
    
    cout << bounds.max.x <<endl;
    
    bounds.min = vec3(std::min(b.min.x, bounds.min.x),
                      std::min(b.min.y, bounds.min.y),
                      std::min(b.min.z, bounds.min.z));
    
    bounds.max = vec3(std::max(b.max.x, bounds.max.x),
                      std::max(b.max.y, bounds.max.y),
                      std::max(b.max.z, bounds.max.z));
  }
}

vec3 Cube::getNormal_Cube(int subIndex, vec3 intersectPosition) {
  return triangles[subIndex].normal;
}






// Sphere


Intersection Cube::intersect_Sphere(Ray ray, int i) {
  vec3 deltap = ray.start - centre;
  float a = glm::dot(ray.dir,ray.dir);
  float b = glm::dot(deltap, ray.dir) * 2;
  float c = glm::dot(deltap, deltap) - (radius * radius);
  
  float disc = b * b - 4 * a * c;
  if (disc < 0) {
    return Intersection(); // No intersection.
  }
  
  disc = sqrt(disc);
  
  float q;
  if (b < 0) {
    q = (-b - disc) * 0.5;
  } else {
    q = (-b + disc) * 0.5;
  }
  
  float r1 = q / a;
  float r2 = c / q;
  
  if (r1 > r2) {
    float tmp = r1;
    r1 = r2;
    r2 = tmp;
  }
  
  float distance = r1;
  if (distance < 0) {
    distance = r2;
  }
  
  if (distance < 0 || isnan(distance)) {
    return Intersection(); // No intersection.
  }
  
  vec3 point = ray.start + (ray.dir * distance * 0.97f);
  vec3 normal = glm::normalize(point - centre);
  
  Intersection intersection = Intersection();
  intersection.position = point;
  intersection.distance = distance;
  intersection.objIndex = i;
  intersection.didIntersect = true;
  intersection.normal = normal;
  intersection.subIndex = -1;
  
  return intersection;
}


vec3 Cube::getNormal_Sphere(int subIndex, vec3 intersectPosition) {
  vec3 N = normalize(intersectPosition - centre);
  return N;
}

void Cube::Update_Bounds_Sphere(){
  return;
}



