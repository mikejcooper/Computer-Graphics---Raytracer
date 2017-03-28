#pragma OPENCL EXTENSION cl_khr_fp64 : enable

int test()
{
	return 1;
}

kernel void accelerate_flow(global int* cells,
                            int accel)
{
  /* get column and row indices */
  int x = get_global_id(0);
  int y = get_global_id(1);
  printf("%d\n", test());



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




// -----------------------------------------------------------------------------------------

