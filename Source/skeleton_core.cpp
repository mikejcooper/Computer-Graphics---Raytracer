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
float focalLength = 500;
vec3 	cameraPos(0.0,0.0,-3);
mat3 	cameraRot(1.0);
vec3 	lightPos( 0, -0.5, -0.7 );
vec3 	lightColor = 14.f * vec3( 1, 1, 1 );
vec3 	indirectLight = 0.5f*vec3( 1, 1, 1 );
void 	Control();
float	turnAngle = (M_PI / 180) * 6;
mat3 	rotationLeft(cos(turnAngle),0,-sin(turnAngle),0,1,0,sin(turnAngle),0,cos(turnAngle));
mat3 	rotationRight(cos(-turnAngle),0,-sin(-turnAngle),0,1,0,sin(-turnAngle),0,cos(-turnAngle));
SDL_Surface* screen;
int t;
vector<Triangle> triangles;


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS   */

void 		Control_LightSource(Uint8* keystate);
void 		Control_Camera(Uint8* keystate);
vec3 		Get_Vector_AtoB(vec3 a,vec3 b);
float		Max_Value(float a,float b);
vec3 		DirectLight( const Intersection& intersection );
mat3 		Cramers_Inverse(mat3 A);
void 		Draw_Triangle_Hit(Intersection& closestIntersection,int x,int y);
vec3 		Cramers_Calculate_Column(mat3 A,int identity_column);
double 	Cramers_Get_Determinant_Variable(mat3 A,int replace_column,int identity_column);
double 	Get_Matrix_Determinant(mat3 A);
void 		Print_Matrix(mat3 A);
bool 		Does_Vector_Intersect_Triangle(vec3 x);
vec3 		Get_TUV_Values(Triangle triangle,vec3 start,vec3 dir);
void 		Control();
void 		Update();
void 		Draw();
bool 		ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection );
mat3 		Cramers_Inverse(mat3 A);

int main( int argc, char* argv[] )
{
	LoadTestModel( triangles );
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}



void Draw()
{
	Intersection closestIntersection;
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 direction(x-SCREEN_WIDTH/2,y-SCREEN_HEIGHT/2,focalLength);
			direction = direction * cameraRot;
			if(ClosestIntersection(cameraPos,direction,closestIntersection)){
				Draw_Triangle_Hit(closestIntersection,x,y);
			}else{
				vec3 black(   0,0,0 );
				PutPixelSDL( screen, x, y, black);
			}
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
bool ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection ){
	bool doesIntersect = false;
	closestIntersection.distance = 50000.00;
	for(int i = 0; i < triangles.size(); i++){
		vec3 x = Get_TUV_Values(triangles[i],start,dir);
		if( Does_Vector_Intersect_Triangle(x))  {
            if(closestIntersection.distance > x.x) {
                closestIntersection.distance = x.x;
                closestIntersection.position = start + x.x * dir;
                closestIntersection.triangleIndex = i;
                doesIntersect = true;
            }
        }
	}
	return doesIntersect;
}

bool Does_Vector_Intersect_Triangle(vec3 x){
	float maxDist = std::numeric_limits<float>::max(); // Check if largest float value can hold intersection distance
	return (x.x < maxDist) && (0 <= x.y && 0 <= x.z && (x.y + x.z) < 1 && 0 <= x.x);
}

vec3 DirectLight( const Intersection& intersection ){
	Intersection nearestTriangle;
	vec3 normal = triangles[intersection.triangleIndex].normal;
	vec3 surfaceToLight = Get_Vector_AtoB(lightPos,intersection.position);
	float radius = length(surfaceToLight);
	float lightIntensity = Max_Value(dot(normal,surfaceToLight),0) / (4 * M_PI * radius * radius);
	ClosestIntersection(lightPos,-surfaceToLight,nearestTriangle);
	if(nearestTriangle.triangleIndex == intersection.triangleIndex){
		return lightColor * lightIntensity;
	} else {
		vec3 zeroLightIntensity = vec3(0,0,0);
		return zeroLightIntensity;
	}
}



vec3 Get_Vector_AtoB(vec3 a,vec3 b){
	return a-b;
}

vec3 Get_TUV_Values(Triangle triangle,vec3 start,vec3 dir){
	// vi represents the vertices of the triangle
	vec3 v0 = triangle.v0;
	vec3 v1 = triangle.v1;
	vec3 v2 = triangle.v2;
	// Triangle co-ordinate system (let v0 be origin)
	vec3 e1 = v1 - v0; // Vector parallel to edge of the triangle between v0 and v1
	vec3 e2 = v2 - v0; // Vector parallel to edge of the triangle between v0 and v2
	vec3 b = start - v0;

	mat3 A( -dir, e1, e2 );
	return glm::inverse( A ) * b;
}

void Draw_Triangle_Hit(Intersection& closestIntersection,int x,int y){
	int triangle = closestIntersection.triangleIndex;
	vec3 color = triangles[triangle].color;
	vec3 lightIntensity = DirectLight(closestIntersection);
	PutPixelSDL( screen, x, y, color * (lightIntensity + indirectLight));
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
	Control();
}

void Control(){
	Uint8* keystate = SDL_GetKeyState( 0 );
	Control_Camera(keystate);
	Control_LightSource(keystate);
}

void Control_LightSource(Uint8* keystate){
	vec3 translateX = vec3(0.1,0  ,0);
	vec3 translateY = vec3(0  ,0.1,0);
	vec3 translateZ = vec3(0  ,0  ,0.1);
	if( keystate[SDLK_w] )
	{
		lightPos += translateZ;
	}
	if( keystate[SDLK_s] )
	{
		lightPos -= translateZ;
	}
	if( keystate[SDLK_a] )
	{
		lightPos -= translateX;
	}
	if( keystate[SDLK_d] )
	{
		lightPos += translateX;
	}
	if( keystate[SDLK_q] )
	{
		lightPos += translateY;
	}
	if( keystate[SDLK_e] )
	{
		lightPos -= translateY;
	}
}

void Control_Camera(Uint8* keystate){
	if( keystate[SDLK_UP] )
	{
	    vec3 translateForward = vec3(0,0,0.1);
		cameraPos += translateForward * cameraRot;
	}
	if( keystate[SDLK_DOWN] )
	{
		vec3 translateForward = vec3(0,0,0.1);
		cameraPos -= translateForward * cameraRot;
	}
	if( keystate[SDLK_LEFT] )
	{
		cameraRot *= rotationLeft;
	}
	if( keystate[SDLK_RIGHT] )
	{
		cameraRot *= rotationRight;
	}
}

mat3 Cramers_Inverse(mat3 A){
	vec3 column1 = Cramers_Calculate_Column(A,0);
	vec3 column2 = Cramers_Calculate_Column(A,1);
	vec3 column3 = Cramers_Calculate_Column(A,2);
	mat3 result(column1,column2,column3);
	return result;
}

vec3 Cramers_Calculate_Column(mat3 A,int identity_column){
	double detA = Get_Matrix_Determinant(A);
	double detX = Cramers_Get_Determinant_Variable(A,0,identity_column);
	double detY = Cramers_Get_Determinant_Variable(A,1,identity_column);
	double detZ = Cramers_Get_Determinant_Variable(A,2,identity_column);
	vec3 xyz(detX/detA,detY/detA,detZ/detA);
	return xyz;
}

double Cramers_Get_Determinant_Variable(mat3 A,int replace_column,int identity_column){
	mat3 identity = mat3(1.0);
	A[replace_column] = identity[identity_column];
	return Get_Matrix_Determinant(A);
}

double Get_Matrix_Determinant(mat3 A){
	double component1 = A[0][0] * (A[1][1]*A[2][2] - A[1][2] * A[2][1]);
	double component2 = A[1][0] * (A[0][1]*A[2][2] - A[0][2] * A[2][1]);
	double component3 = A[2][0] * (A[0][1]*A[1][2] - A[0][2] * A[1][1]);
	return component1 + (-component2) + component3;
}

void Print_Matrix(mat3 A){
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			printf("%f,",A[j][i] );
		}
		printf("\n");
	}
}

float Max_Value(float a,float b){
	if(a > b){
		return a;
	} else {
		return b;
	}
}


