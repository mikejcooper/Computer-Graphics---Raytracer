#pragma OPENCL EXTENSION cl_khr_fp64 : enable


#ifndef __OPENCL_VERSION__
#include <stdlib.h>
#include <glm/glm.hpp>
#include <vector>
// etc
#endif

// using namespace std;
// using glm::vec3;

int test()
{
	return 1;
}

kernel void accelerate_flow(global glm::vec3* v0,
                            int accel)
{
  /* get column and row indices */
  int x = get_global_id(0);
  int y = get_global_id(1);
  printf("%d\n", test());

  v0.x = 7;



 //  vec3 direction(x-SCREEN_WIDTH/2,y-SCREEN_HEIGHT/2,focalLength);
	// direction = direction * cameraRot;

	// if(ClosestIntersection(cameraPos,direction,closestIntersection)) {
	// 	Draw_Triangle_Hit(closestIntersection,x,y);
	// }
	// else {
	// 	vec3 black(   0,0,0 );
	// 	PutPixelSDL( screen, x, y, black);
	// }
}


/*
  ClosestIntersection takes the start position of the ray and its direction and a std::vector of triangles 
  as input. It should then check for intersection against all these triangles. If an 
  intersection occurred it should return true. Otherwise false. In the case of an intersection 
  it should also return some information about the closest intersection. 
*/
// bool ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection ){
//   bool doesIntersect = false;
//   closestIntersection.distance = 50000.00;
//   for(int i = 0; i < triangles.size(); i++){
//     vec3 x = Get_TUV_Values(triangles[i],start,dir);
//     if( Does_Vector_Intersect_Triangle(x))  {
//             if(closestIntersection.distance > x.x) {
//                 closestIntersection.distance = x.x;
//                 closestIntersection.position = start + x.x * dir;
//                 closestIntersection.triangleIndex = i;
//                 doesIntersect = true;
//             }
//         }
//   }
//   return doesIntersect;
// }


// void Draw_Triangle_Hit(Intersection& closestIntersection,int x,int y){
//   int triangle = closestIntersection.triangleIndex;
//   vec3 color = triangles[triangle].color;
//   vec3 lightIntensity = DirectLight(closestIntersection);
//   PutPixelSDL( screen, x, y, color * (lightIntensity + indirectLight));
// }



// -----------------------------------------------------------------------------------------

