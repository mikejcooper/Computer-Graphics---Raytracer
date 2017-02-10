#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <limits.h>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

using namespace std;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* STRUCTURES                                                           */
struct Intersection
{
    glm::vec3 position;
    float distance;
    int triangleIndex;
};

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
mat3 Cramers_Inverse(mat3 A);

int main( int argc, char* argv[] )
{
	LoadTestModel( triangles );
	Intersection closestIntersection;
	// Initialise Ray
	vec3 s(5,60,5); // Posisiton s
	vec3 d(1,2,6); // Direction d
	bool state = ClosestIntersection(s,d,triangles,closestIntersection);
	

	// screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	// t = SDL_GetTicks();	// Set start value for timer.

	// while( NoQuitMessageSDL() )
	// {
	// 	Update();
	// 	Draw();
	// }

	// SDL_SaveBMP( screen, "screenshot.bmp" );
	// return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 color( 1.0, 0.0, 0.0 );
			PutPixelSDL( screen, x, y, color );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

/*
	ClosestIntersection takes the start position of the ray and its direction and a std::vector of triangles 
	as input. It should then check for intersection against all these triangles. If an 
	intersection occurred it should return true. Otherwise false. In the case of an intersection 
	it should also return some information about the closest intersection. 
*/
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection ){
	bool intersection = false;
	float maxDist = std::numeric_limits<float>::max(); // Check if largest float value can hold intersection distance

	for(int i = 0; i < triangles.size(); i++){
		// vi represents the vertices of the triangle
		vec3 v0 = triangles[i].v0;
		vec3 v1 = triangles[i].v1;
		vec3 v2 = triangles[i].v2;
		// Triangle co-ordinate system (let v0 be origin)
		vec3 e1 = v1 - v0; // Vector parallel to edge of the triangle between v0 and v1
		vec3 e2 = v2 - v0; // Vector parallel to edge of the triangle between v0 and v2
		vec3 b = start - v0;

		mat3 A( -dir, e1, e2 );
		vec3 x = Cramers_Inverse( A ) * b;
		// Max Float check && Constraints in equations 7,8,9,11
		// t = x.x, u = x.y, v = x.z
		if( (x.x < maxDist) && (0 < x.y && 0 < x.z && (x.y + x.z) < 1 && 0 <= x.x))  {
            if(closestIntersection.distance > t) {
                closestIntersection.position = x;
                closestIntersection.distance = x.x;
                closestIntersection.triangleIndex = i;
                intersection = true;
            }
        }
	}
	return intersection;
}

mat3 Cramers_Inverse(mat3 A){
	// todo: calculate a closed form solution for the inverse of the matrix
	/*
		This can be done with Cramer’s rule. We need this re- sult if we would like to 
		increase the speed of the intersection computations, which will be the bottleneck 
		of our program. Then, when we have the closed form solution we can postpone the 
		computation of some of the matrix ele- ments, until they are needed. We start by 
		just computing the row needed to compute the distance t. Then, if t is negative there 
		is no need to continue analyzing that intersection, as it will not occur.
	*/
	return glm::inverse( A );
}



