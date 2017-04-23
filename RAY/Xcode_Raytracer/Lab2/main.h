#include <iostream>
#include "glm/glm.hpp"
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "Materials.hpp"
#include <limits.h>
#include "glm/gtc/type_ptr.hpp"
#include <math.h>
#include "Intersection.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "Control.hpp"
#include "Light.hpp"





using namespace std;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* STRUCTURES                                                           */




/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES  */


//vec3  cameraPos(0.0,0.0,-3);
//mat3  cameraRot(1.0);
Camera camera;
Control control;

//vec3  lightPos( 0, -0.5, -0.7 );
//vec3  lightColor = 14.0f * vec3( 1, 1, 1 );
vec3  indirectLight = 0.5f * vec3( 1, 1, 1 );

int   MAX_DEPTH = 5;
int   NULLobjectIndex = -1;

//bool  MOVEMENT = true;
//int   SOFT_SHADOWS_SAMPLES = 1;
//int   AA_SAMPLES = 1;
//int   DOF_VALUE = 1;
//bool  SHOW_EDGES = false;
//bool  DOF = false;

/* Materials */
float AIR_REFRACTIVE_INDEX = 1.0;
float GLASS_REFRACTIVE_INDEX = 1.52;
float DIFFUSE_SPECULAR_REFLECTION = 0.18;

SDL_Surface* screen;
const int   SCREEN_WIDTH = 500;
const int   SCREEN_HEIGHT = 500;
const float FOCAL_LENGTH = 500;
const float TURN_ANGLE = (M_PI / 180) * 6;

const mat3  rotationUp(1,0,0,0,cos(TURN_ANGLE),-sin(TURN_ANGLE),0,sin(TURN_ANGLE),cos(TURN_ANGLE));
const mat3  rotationDown(1,0,0,0,cos(-TURN_ANGLE),-sin(-TURN_ANGLE),0,sin(-TURN_ANGLE),cos(-TURN_ANGLE));
const mat3  rotationLeft(cos(TURN_ANGLE),0,-sin(TURN_ANGLE),0,1,0,sin(TURN_ANGLE),0,cos(TURN_ANGLE));
const mat3  rotationRight(cos(-TURN_ANGLE),0,-sin(-TURN_ANGLE),0,1,0,sin(-TURN_ANGLE),0,cos(-TURN_ANGLE));

vector<Object> Objects;
vector<Triangle> Triangles;
vector<Light> Lights;
vec3 screenPixels[SCREEN_HEIGHT][SCREEN_WIDTH];


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS   */
/* ----------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS   */

void    Control1();
void    Control_LightSource(Uint8* keystate);
void    Control_Camera(Uint8* keystate);
void    Control_Features(Uint8* keystate);

int     Update(int t);
void    Draw();

bool    Intersects(vec3 x);
bool    ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection );
vec3    Calculate_Intersection(Triangle triangle,vec3 start,vec3 dir);

vec3    DirectLight( const Intersection& intersection );
void    SoftShadowPositions(vec3 positions[]);
void    AASampling(int pixelx, int pixely);
vec3    AASuperSampling(float pixelx, float pixely);
vec3    traceRayFromCamera(float x , float y);
bool    EdgeDectection(vec3 current_color, vec3 average_color);
vec3    AddVectorAndAverage(vec3 A, vec3 B);
void    finish();
void    Calculate_DOF();
vec3    getColor(Ray ray, int depth);


void SOFT_SHADOWS_SAMPLES_INC();
void SOFT_SHADOWS_SAMPLES_DEC();
void AA_SAMPLES_INC();
void AA_SAMPLES_DEC();
void DOF_SAMPLES_INC();
void DOF_SAMPLES_DEC();

float max(float a,float b);





float max(float a,float b){
  if(a > b){
    return a;
  } else {
    return b;
  }
}
