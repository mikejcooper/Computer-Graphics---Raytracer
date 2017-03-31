#include "Raytracer.h"
#include <list>
#include <tuple>
#include <string>

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

vec3  cameraPos(0.0,0.0,-3);
mat3  cameraRot(1.0);
vec3  lightPos( 0, -0.5, -0.7 );
vec3  lightColor = 14.0f * vec3( 1, 1, 1 );
vec3  indirectLight = 0.5f * vec3( 1, 1, 1 );

vector<Triangle> Triangles;


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS   */

void    Control();
void    Control_LightSource(Uint8* keystate);
void    Control_Camera(Uint8* keystate);
void    Control_Features(Uint8* keystate);

int    Update(int t);
void    Draw();

bool    Intersects(vec3 x);
bool    ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection );
vec3    Calculate_Intersection(Triangle triangle,vec3 start,vec3 dir);

vec3    DirectLight( const Intersection& intersection );
void    SoftShadowPositions(vec3 positions[]);
vec3 AASampling(int pixelx, int pixely);
vec3 AASuperSampling(float pixelx, float pixely);
vec3 AAEdgeSampling(float pixelx, float pixely);
vec3 traceRayFromCamera(float x , float y);
void finish();

int main( int argc, char* argv[] )
{
  LoadTestModel( Triangles );
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  int t = SDL_GetTicks(); // Set start value for timer.

  // while( NoQuitMessageSDL() )
  // {
    t = Update(t);
    Draw();
  // }

  SDL_SaveBMP( screen, "screenshot1.bmp" );
  return 0;
}


void initalise(){
  
}

list<tuple<int, int>> edges;

void Draw()
{
  if( SDL_MUSTLOCK(screen) )
    SDL_LockSurface(screen);

  #pragma omp parallel for schedule(auto)
  for( int y=0; y<SCREEN_HEIGHT; y++ ) {
    for( int x=0; x<SCREEN_WIDTH; x++ ) {
      vec3 pixel_color = AASampling(x, y);

      PutPixelSDL( screen, x, y, pixel_color);
    }
  }

  for(tuple<int, int> i : edges) {
    int x = get<0>(i);
    int y = get<1>(i);
    PutPixelSDL( screen, x, y, vec3(0,0,0));

  }

  if( SDL_MUSTLOCK(screen) )
    SDL_UnlockSurface(screen);

  SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

/*
  ClosestIntersection takes the start position of the ray and its direction and a std::vector of Triangles 
  as input. It should then check for intersection against all these Triangles. If an 
  intersection occurred it should return true. Otherwise false. In the case of an intersection 
  it should also return some information about the closest intersection. 
*/
bool ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection ){
  bool doesIntersect = false;
  closestIntersection.distance = 50000.00;

  // For all Triangles in model
  for(int i = 0; i < Triangles.size(); i++){
    vec3 x = Calculate_Intersection(Triangles[i], start, dir);
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
    vec3 normal = glm::normalize(Triangles[intersection.triangleIndex].normal);
    // Direct light intensity given distance/radius
    float lightIntensity = max( dot(normal, surfaceToLight) , 0 ) / (4 * M_PI * radius * radius);

    Intersection nearestTriangle;
    ClosestIntersection(positions[k],-surfaceToLight,nearestTriangle);
    
    if (nearestTriangle.triangleIndex != intersection.triangleIndex){
      // If intersection is closer to light source than self
      if (nearestTriangle.distance < radius*0.99f)
        lightIntensity = 0; // Zero light intensity 
    } 
    
    light += ( lightColor / (float) SOFT_SHADOWS_SAMPLES ) * lightIntensity;
  }
  return light + indirectLight;
}

void SoftShadowPositions(vec3 positions[]){
  // Set first ray projection to be at 'light position'
  positions[0] = lightPos;
  // Settings to handle variable number of soft shadows
  float mul3 = (float) (SOFT_SHADOWS_SAMPLES - 1) / 3;
  float shift = (mul3 < 1) ? 0.007 : (mul3 < 4) ? 0.005 : (mul3 < 6) ? 0.004 : (mul3 < 11) ? 0.003 : 0.002 ;

  // Find equal positions around light source 
  for(int i = 1; i < SOFT_SHADOWS_SAMPLES; i++) {
    float sign  = (i % 6 >= 3) ? -1 : 1;            // Rays from +/- 
    int mod = i % 3;                                // Rays from (x,y,z) 
    if (mod == 0){
      positions[i] = lightPos + vec3(shift, 0, 0) * sign * (float) (i + 1.0f); // Shift in X
    }
    else if(mod == 1){
      positions[i] = lightPos + vec3(0, shift, 0) * sign * (float) i;          // Shift in Y
    }
    else {
      positions[i] = lightPos + vec3(0, 0, shift) * sign * (float) (i - 1.0f); // Shift in Z
    }
  }
}

vec3 AASampling(int pixelx, int pixely) {
  vec3 pixel_color(0,0,0);
  bool resample = false;

  for( int k=0; ( k < AA_SAMPLES ) && !resample ; k++ ){

    float x1 = (k % 2 == 0) ? pixelx + 0.3*k : pixelx - 0.3*k;

    for( int m=0; (m < AA_SAMPLES) && !resample; m++ ){ 

      float y1 = (m % 2 == 0) ? pixely + 0.3*m : pixely - 0.3*m;

      vec3 local_color = traceRayFromCamera(x1, y1);

      float threshold = 0.03;

      // Edge dectection using current and previous aliasing points colour difference. 
      if ( (((local_color.z - pixel_color.z > threshold ) || (local_color.y - pixel_color.y > threshold ) || 
            (local_color.x - pixel_color.x > threshold )) && pixel_color.x != 0) || resample){
        cout << "HERE";
        // resample at these points with more accuracy  
        resample = true;
      }
      else if (k > 2 && !resample) {
        return local_color;
      } 
      pixel_color = (m == 0) ?  local_color : ( (pixel_color + local_color) / 2.0f ) ;
    }
  }

  if (resample) {
    edges.push_back( tuple<int,int>(pixelx, pixely) );
  }

  pixel_color = (resample) ? AASuperSampling(pixelx, pixely) : pixel_color;
  return pixel_color;
}

// vec3 AAEdgeSampling(float pixelx, float pixely){
//   vec3 pixel_color(0,0,0);
//   for( int k=0; k<AA_SAMPLES; k++ ){

//     float x1 = (k % 2 == 0) ? pixelx + 0.02*k : pixelx - 0.02*k;

//     for( int m=0; m<AA_SAMPLES*4; m++ ){ 

//       float y1 = (m % 2 == 0) ? pixely + 0.02*m : pixely - 0.02*m;

//       vec3 local_color = traceRayFromCamera(x1, y1);

//       pixel_color = (m == 0) ?  local_color : ( (pixel_color + local_color) / 2.0f ) ;
//     }
//   }
//   return pixel_color;

// }


vec3 AASuperSampling(float pixelx, float pixely){
  vec3 pixel_color(0,0,0);

  for( int k=0; k<AA_SAMPLES*4; k++ ){

    float x1 = (k % 2 == 0) ? pixelx + 0.02*k : pixelx - 0.02*k;

    for( int m=0; m<AA_SAMPLES*4; m++ ){ 

      float y1 = (m % 2 == 0) ? pixely + 0.02*m : pixely - 0.02*m;

      vec3 local_color = traceRayFromCamera(x1, y1);

      pixel_color = (m == 0) ?  local_color : ( (pixel_color + local_color) / 2.0f ) ;
    }
  }
  return pixel_color;
}



vec3 traceRayFromCamera(float x , float y) {
  Intersection closestIntersection;
  // Get direction of camera
  vec3 direction(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, FOCAL_LENGTH);
  direction = direction * cameraRot;
  // If ray casting from camera position hits a triangle
  if(ClosestIntersection(cameraPos, direction, closestIntersection)) {
    // Identify triangle, find colour and 'Put' corrisponding pixel
    int index = closestIntersection.triangleIndex;
    vec3 color = Triangles[index].color;
    vec3 lightIntensity = DirectLight(closestIntersection);
    return color * lightIntensity;
  } 
  else {
    return vec3( 0,0,0 );
  }
}


/* ----------------------------------------------------------------------------*/
/* CONTROLS                                                                    */
/* ----------------------------------------------------------------------------*/

int Update(int t)
{
  // Compute frame time:
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  cout << "Render time: " << dt << " ms." << endl;
  Control();
  return t;
}

void Control(){
  Uint8* keystate = SDL_GetKeyState( 0 );
  if (MOVEMENT){
    Control_Camera(keystate);
    Control_LightSource(keystate);
  }
  Control_Features(keystate);
}

void Control_LightSource(Uint8* keystate){
  vec3 translateX = vec3(0.1,0  ,0);
  vec3 translateY = vec3(0  ,0.1,0);
  vec3 translateZ = vec3(0  ,0  ,0.1);

  if( keystate[SDLK_w] ) {
    lightPos += translateZ;
  }
  if( keystate[SDLK_s] ) {
    lightPos -= translateZ;
  }
  if( keystate[SDLK_a] ) {
    lightPos -= translateX;
  }
  if( keystate[SDLK_d] ) {
    lightPos += translateX;
  }
  if( keystate[SDLK_q] ) {
    lightPos += translateY;
  }
  if( keystate[SDLK_e] ) {
    lightPos -= translateY;
  }
}

void Control_Camera(Uint8* keystate){
  if( keystate[SDLK_UP] ) {
      vec3 translateForward = vec3(0,0,0.1);
    cameraPos += translateForward * cameraRot;
  }
  if( keystate[SDLK_DOWN] ) {
    vec3 translateForward = vec3(0,0,0.1);
    cameraPos -= translateForward * cameraRot;
  }
  if( keystate[SDLK_LEFT] ) {
    cameraRot *= rotationLeft;
  }
  if( keystate[SDLK_RIGHT] ) {
    cameraRot *= rotationRight;
  }
  if( keystate[SDLK_UP] && keystate[SDLK_RALT] ) {
    cameraRot *= rotationUp;
  }
  if( keystate[SDLK_DOWN] && keystate[SDLK_RALT] ) {
    cameraRot *= rotationDown;
  }
  if( keystate[SDLK_r] && keystate[SDLK_RALT] ) {
    cameraPos = vec3(0.0,0.0,-3);
  }
}


void Control_Features(Uint8* keystate){
  while(keystate[SDLK_LALT]) {
    SDL_PumpEvents(); // update key state array
    if(keystate[SDLK_s] && keystate[SDLK_EQUALS] ){
      SOFT_SHADOWS_SAMPLES_INC();
      while (keystate[SDLK_EQUALS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_s] && keystate[SDLK_MINUS] ){
      SOFT_SHADOWS_SAMPLES_DEC();
      while (keystate[SDLK_MINUS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_a] && keystate[SDLK_EQUALS] ){
      AA_SAMPLES_INC();
      while (keystate[SDLK_EQUALS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_a] && keystate[SDLK_MINUS] ){
      AA_SAMPLES_DEC();
      while (keystate[SDLK_MINUS]) SDL_PumpEvents();
    }
    if(keystate[SDLK_m]){
      MOVEMENT = (MOVEMENT) ? false : true; 
      cout << "Movement " << MOVEMENT << endl;
      while (keystate[SDLK_m]) SDL_PumpEvents();
    }
    if(keystate[SDLK_q]){
      finish();
    }
  }
}

void finish(){
  SDL_SaveBMP( screen, "screenshot1.bmp" );
  exit(0);
}
