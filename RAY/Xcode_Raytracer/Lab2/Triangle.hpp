//
//  Triangle.hpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include <stdio.h>
#include "glm/glm.hpp"


using namespace std;
using namespace glm;

class Triangle
{
public:
  
  vec3 v0;
  vec3 v1;
  vec3 v2;
  vec3 normal;
  vec3 color;
  
  Triangle( vec3 v0, vec3 v1, vec3 v2, vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
  {
    ComputeNormal();
  }
  
  void ComputeNormal()
  {
    vec3 e1 = v1-v0;
    vec3 e2 = v2-v0;
    normal = normalize( cross( e2, e1 ) );
  }
};

#endif /* Triangle_hpp */
