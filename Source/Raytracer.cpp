#include "Raytracer.h"


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

vec3  cameraPos(0.0,0.0,-3);
mat3  cameraRot(1.0);
vec3  lightPos( 0, -0.5, -0.7 );
vec3  lightColor = 14.0f * vec3( 1, 1, 1 );
vec3  indirectLight = 0.5f * vec3( 1, 1, 1 );

vector<Triangle> Triangles;
vec3 screenPixels[SCREEN_HEIGHT][SCREEN_WIDTH];



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
void AASampling(int pixelx, int pixely);
vec3 AASuperSampling(float pixelx, float pixely);
vec3 traceRayFromCamera(float x , float y);
bool EdgeDectection(vec3 current_color, vec3 average_color);
void finish();

int main( int argc, char* argv[] )
{
  LoadTestModel( Triangles );
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  int t = SDL_GetTicks(); // Set start value for timer.

  while( NoQuitMessageSDL() )
  {
    t = Update(t);
    Draw();
  }

  SDL_SaveBMP( screen, "screenshot1.bmp" );
  return 0;
}


void initalise(){
  
}


void Draw()
{
  if( SDL_MUSTLOCK(screen) )
    SDL_LockSurface(screen);

  #pragma omp parallel for schedule(auto)
  for( int y=0; y<SCREEN_HEIGHT; y++ ) {
    for( int x=0; x<SCREEN_WIDTH; x++ ) {
      
      screenPixels[x][y] = traceRayFromCamera(x, y);
      
      if (AA_SAMPLES > 1){
        AASampling(x, y);
      }      
      PutPixelSDL( screen, x, y, screenPixels[x][y]);
    }
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
      if (nearestTriangle.distance < radius * 0.99f)
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

void AASampling(int pixelx, int pixely) {
  vec3 average_color(0,0,0);
  bool resample = false;
  float pixel_distance = 1.2;
  float steps = 1;  // Actual steps = steps + 1 
  
  for( float k=0, b1=0; ( k <= pixel_distance ) && !resample ; k += (pixel_distance / steps), b1++ ){

    float x1 = ( (int) b1 == 0 ) ? pixelx + k : pixelx - k;   

    for( float m=0, b2=0; ( m <= pixel_distance ) && !resample; m += (pixel_distance / steps), b2++ ){ 

      float y1 = ( (int) b2 % 2 == 0 )  ? pixely + m : pixely - m;

      vec3 current_color = traceRayFromCamera(x1, y1);

      // Edge dectection using current and previous aliasing points colour difference. 
      if ( EdgeDectection(current_color, average_color) ){
        // resample at these points with more accuracy  
        resample = true;
      }
      else if (k > pixel_distance / steps) {
        screenPixels[pixelx][pixely] = (average_color + current_color) / 2.0f;
        return;
      } 

      average_color = (m == 0) ?  current_color : ( (average_color + current_color) / 2.0f ) ;
    }
  }

  if (SHOW_EDGES){
      average_color = (resample) ? vec3(0,0,0) : average_color;
  } else {
      average_color = (resample) ? AASuperSampling(pixelx, pixely) : average_color;
  }

  screenPixels[pixelx][pixely] = average_color;
}

vec3 AASuperSampling(float pixelx, float pixely){
  vec3 average_color(0,0,0);

  float pixel_distance = 0.8;
  float steps = 5;

  // if(AA_SAMPLES < 3){
  //   pixel_distance = 1;
  //   steps = 10;
  // } else {
  //   pixel_distance = 1.2;
  //   steps = 15;
  // }
  
  for( float k=0, b1=0; ( k <= pixel_distance ) ; k += (pixel_distance / steps), b1++ ){
    float x1 = ( (int) b1 % 2 == 0 ) ? pixelx + k : pixelx - k;
    for( float m=0, b2=0; ( m <= pixel_distance ) ; m += (pixel_distance / steps), b2++ ){
      float y1 = ( (int) b2 % 2 == 0 )  ? pixely + m : pixely - m;
      
      vec3 current_color = traceRayFromCamera(x1, y1);
      
      average_color = (m == 0) ?  current_color : ( (average_color + current_color) / 2.0f ) ;
    }    
  }
  return average_color;
}

bool EdgeDectection(vec3 current_color, vec3 average_color){
  float threshold = 0.06;
  float x = abs(current_color.x - average_color.x);
  float y = abs(current_color.y - average_color.y);
  float z = abs(current_color.z - average_color.z);
  if ( ( x > threshold  || y > threshold  || z > threshold  ) && average_color.x != 0){
    return true;
  }
  else {
    return false;
  }
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
    if(keystate[SDLK_e]){
      SHOW_EDGES = (SHOW_EDGES) ? false : true; 
      cout << "Show Edges " << SHOW_EDGES << endl;
      while (keystate[SDLK_e]) SDL_PumpEvents();
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
