//
//  KDTree.hpp
//  Raytracer
//
//  Created by Mike Cooper on 30/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef KDTree_hpp
#define KDTree_hpp

#include <math.h>
#include <vector>
#include <stdio.h>
#include "Boundaries.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"
#include "Object.hpp"
#include "Cube.hpp"

//class Object;
//class Intersection;

#define MIN_OBJECT_COUNT 20

class KDTree {
private:
  int depth;
  int axisRetries;
  char axis;
  Boundaries bounds;
  std::vector<Cube*> objects;
  KDTree* left;
  KDTree* right;
  
  void build();
  char toggleAxis();
  
public:
  
  KDTree(int depth_, char axis_, std::vector<Cube*> objects_) :
  depth(depth_), axis(axis_), objects(objects_)
  {
    axisRetries = 0;
    left = NULL;
    right = NULL;
    build();
  }
  
  Intersection getClosestIntersection(const Ray);
  Intersection getClosestObjectIntersection(const Ray);
};

#endif /* KDTree_hpp */
