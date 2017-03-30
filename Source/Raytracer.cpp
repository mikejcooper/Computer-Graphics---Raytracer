#include "Raytracer.h"

#define NUM_LIGHTS 10
#define SOFT_SHADOWS_SAMPLES 1

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
vec3  cameraPos(0.0,0.0,-3);
mat3  cameraRot(1.0);
vec3  lightPos( 0, -0.5, -0.7 );
vec3  lightColor = 14.f * vec3( 1, 1, 1 );
vec3  indirectLight = 0.5f*vec3( 1, 1, 1 );
void  Control();
float turnAngle = (M_PI / 180) * 6;
mat3  rotationUp(1,0,0,0,cos(turnAngle),-sin(turnAngle),0,sin(turnAngle),cos(turnAngle));
mat3  rotationDown(1,0,0,0,cos(-turnAngle),-sin(-turnAngle),0,sin(-turnAngle),cos(-turnAngle));
mat3  rotationLeft(cos(turnAngle),0,-sin(turnAngle),0,1,0,sin(turnAngle),0,cos(turnAngle));
mat3  rotationRight(cos(-turnAngle),0,-sin(-turnAngle),0,1,0,sin(-turnAngle),0,cos(-turnAngle));
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vector<Intersection> closestIntersections;



/* ----------------------------------------------------------------------------*/
/* FUNCTIONS   */

void    Control_LightSource(Uint8* keystate);
void    Control_Camera(Uint8* keystate);
float   max(float a,float b);
vec3    DirectLight( const Intersection& intersection );
double  Get_Matrix_Determinant(mat3 A);
void    Print_Matrix(mat3 A);
bool    Intersects(vec3 x);
vec3    Calculate_Intersection(Triangle triangle,vec3 start,vec3 dir);
void    Control();
void    Update();
void    Draw();
bool    ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection );
mat3    Cramers_Inverse(mat3 A);
void    Feature_Controls(Uint8* keystate);
void SoftShadowPositions(vec3 positions[]);


bool isUpdated = true; // todo

Light lights[NUM_LIGHTS];


int main( int argc, char* argv[] )
{
  LoadTestModel( triangles );
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  t = SDL_GetTicks(); // Set start value for timer.

  while( NoQuitMessageSDL() )
  {

    Update();
    Draw();
    

  }

  SDL_SaveBMP( screen, "screenshot1.bmp" );
  return 0;
}


void initalise(){
  
}


void Draw()
{
  Intersection closestIntersection;
  if( SDL_MUSTLOCK(screen) )
    SDL_LockSurface(screen);

  #pragma omp parallel for schedule(auto)
  for( int y=0; y<SCREEN_HEIGHT; ++y )
  {
    for( int x=0; x<SCREEN_WIDTH; ++x )
    {
      // Get direction of camera
      vec3 direction(x-SCREEN_WIDTH/2,y-SCREEN_HEIGHT/2,focalLength);
      direction = direction * cameraRot;
      // If ray casting from camera position hits a triangle
      if(ClosestIntersection(cameraPos, direction, closestIntersection)){
        // Identify triangle, find colour and 'Put' corrisponding pixel
        int index = closestIntersection.triangleIndex;
        vec3 color = triangles[index].color;
        vec3 lightIntensity = DirectLight(closestIntersection);
        PutPixelSDL( screen, x, y, color * lightIntensity);
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

  // For all triangles in model
  for(int i = 0; i < triangles.size(); i++){
    vec3 x = Calculate_Intersection(triangles[i], start, dir);
    if( Intersects(x))  {
      // If the current intersection is closer than previous, update
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

bool Intersects(vec3 x){
  // Check if largest float value can hold intersection distance
  float maxDist = std::numeric_limits<float>::max(); 
  // Check x statisfy rules for intersection
  return (x.x <= maxDist) && (0 <= x.y && 0 <= x.z && 0 <= x.x && (x.y + x.z) <= 1);
}

vec3 Calculate_Intersection(Triangle triangle, vec3 start, vec3 dir){
  // vi represents the vertices of the triangle
  vec3 v0 = triangle.v0;
  vec3 v1 = triangle.v1;
  vec3 v2 = triangle.v2;
  
  vec3 e1 = v1 - v0;    // Vector parallel to edge of the triangle between v0 and v1
  vec3 e2 = v2 - v0;    // Vector parallel to edge of the triangle between v0 and v2
  vec3 b = start - v0;  // Vector parallel to edge between v0 and camara position  


  // Cramer's rule: faster than   // mat3 A( -dir, e1, e2 ); return glm::inverse( A ) * b;
  vec3 cross_e1e2 = glm::cross(e1,e2);
  vec3 cross_be2 = glm::cross(b,e2);
  vec3 cross_e1b = glm::cross(e1,b);

  float dot_e1e2b = glm::dot(cross_e1e2, b);

  float dot_e1e2d = glm::dot(cross_e1e2, -dir);
  float dot_be2d =  glm::dot(cross_be2, -dir);
  float dot_e1bd =  glm::dot(cross_e1b, -dir);

  // Point of intersection: x = (t, u, v), from v0 + ue1 + ve2 = s + td
  vec3 x = vec3(dot_e1e2b / dot_e1e2d, dot_be2d / dot_e1e2d, dot_e1bd / dot_e1e2d);
  
  return x;
}


vec3 DirectLight( const Intersection& intersection ){
  vec3 light(0.0f,0.0f,0.0f);
  vec3 positions[100];
  SoftShadowPositions(positions);



  for (int k = 0; k < SOFT_SHADOWS_SAMPLES; k++) {



    // Unit vector from point of intersection to light
    vec3 surfaceToLight = glm::normalize(positions[k] - intersection.position);
    // Distance from point of intersection to light
    float radius = length(positions[k] - intersection.position);
    // Unit vector perpendicular to plane.
    vec3 normal = glm::normalize(triangles[intersection.triangleIndex].normal);
    // Direct light intensity given distance/radius
    float lightIntensity = max( dot(normal, surfaceToLight) , 0 ) / (4 * M_PI * radius * radius);

    Intersection nearestTriangle;
    ClosestIntersection(positions[k],-surfaceToLight,nearestTriangle);
    
    if (nearestTriangle.triangleIndex != intersection.triangleIndex){
      // If intersection is closer to light source than self
      if (nearestTriangle.distance < radius*0.99f)
        lightIntensity = 0; // Zero light intensity 
    } 
    
    // ( lightColor /= (float) SOFT_SHADOWS_SAMPLES )

    light += ( lightColor / (float) SOFT_SHADOWS_SAMPLES ) * lightIntensity;
  }
  return light + indirectLight;
}

void SoftShadowPositions(vec3 positions[]){
  float factor = (float) (SOFT_SHADOWS_SAMPLES - 1) / 3;
  float shift = 0.003;
  positions[0] = lightPos;

  if(factor < 1) {
    shift = 0.007;
  } 
  else if (factor < 4) {
    shift = 0.005;
  }
  else if (factor < 6) {
    shift = 0.003;
  }
  else if (factor < 10) {
    shift = 0.001;
  }
  else {
    cout << "Soft shadow count OVERFLOW";
    return;
  }

  for(int i = 1; i < SOFT_SHADOWS_SAMPLES; i++) {
    int mod = i % 3;
    if (mod == 0){
      positions[i] = lightPos + vec3(shift, 0, 0) * (float) (i + 1.0f); // Translate X
    }
    else if(mod == 1){
      positions[i] = lightPos + vec3(0, shift, 0) * (float) i; // Translate Y
    }
    else {
      positions[i] = lightPos + vec3(0, 0, shift) * (float) (i - 1.0f);  // Translate Z
    }
  }
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
  Feature_Controls(keystate);
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
  if( keystate[SDLK_UP] && keystate[SDLK_LALT] )
  {
    cameraRot *= rotationUp;
  }
  if( keystate[SDLK_DOWN] && keystate[SDLK_LALT] )
  {
    cameraRot *= rotationDown;
  }
  if( keystate[SDLK_r] && keystate[SDLK_LALT] )
  {
    cameraPos = vec3(0.0,0.0,-3);
  }
}

void Feature_Controls(Uint8* keystate){
  if( keystate[SDLK_l] && keystate[SDLK_EQUALS] )
    {
      printf("%s\n", "here");
    }
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


