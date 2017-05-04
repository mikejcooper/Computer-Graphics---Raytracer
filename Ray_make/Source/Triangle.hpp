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
#include "Boundaries.hpp"


//using namespace std;
using namespace glm;

class Triangle
{
public:
  
  vec3 v0;
  vec3 v1;
  vec3 v2;
  vec3 normal;
  vec3 color;
  Boundaries bounds;

  
  Triangle( vec3 v0, vec3 v1, vec3 v2, vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
  {
    bounds.min = vec3(fmin(v0.x, fmin(v1.x, v2.x)),
                        fmin(v0.y, fmin(v1.y, v2.y)),
                        fmin(v0.z, fmin(v1.z, v2.z)));
    
    bounds.max = vec3(fmax(v0.x, fmax(v1.x, v2.x)),
                        fmax(v0.y, fmax(v1.y, v2.y)),
                        fmax(v0.z, fmax(v1.z, v2.z)));
    

    
    ComputeNormal();
  }
  
  void ComputeNormal()
  {
    vec3 e1 = v1-v0;
    vec3 e2 = v2-v0;
    normal = normalize( cross( e2, e1 ) );
  }
  
  void Update_Bounds(){
    bounds.min = vec3(fmin(v0.x, fmin(v1.x, v2.x)),
                      fmin(v0.y, fmin(v1.y, v2.y)),
                      fmin(v0.z, fmin(v1.z, v2.z)));
    
    bounds.max = vec3(fmax(v0.x, fmax(v1.x, v2.x)),
                      fmax(v0.y, fmax(v1.y, v2.y)),
                      fmax(v0.z, fmax(v1.z, v2.z)));
  }
  
};

#endif /* Triangle_hpp */
