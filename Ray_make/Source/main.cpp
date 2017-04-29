#include "main.h"
#include "Intersection.hpp"
#include "Ray.hpp"
#include "Raytracer.hpp"
#include "Camera.hpp"
#include "Control.hpp"



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
  Uint8* keystate = SDL_GetKeyState( 0 );
  control.Update(keystate);
  return t;
}


int main( int argc, char* argv[] )
{
//    LoadGenericmodel(Objects);
  LoadTestModel( &Objects );
  

  
  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  int t = SDL_GetTicks(); // Set start value for timer.
  
//  Lights.push_back(Light( vec3( 0, 0.6, 0 ) ));
//  camera = Camera( vec3(0.0,1.2,-2), mat3(1.0) );
  
  Lights.push_back(Light());
  camera = Camera( vec3(0.0,0.0,-3), mat3(1.0) );


  control = Control(&Lights[0].position, &camera);
  
  
  
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
      
      if (control.AA_SAMPLES > 1 && x > 1 && y > 1){
        AASampling(x, y);
      } else {
        screenPixels[x][y] = traceRayFromCamera(x, y);
      }
      
      PutPixelSDL( screen, x, y, screenPixels[x][y]);
    }
  }
  
  
  if (control.DOF_VALUE > 1){
    Calculate_DOF();
  }
  
  
  for( int y=0; y<SCREEN_HEIGHT; y++ ) {
    for( int x=0; x<SCREEN_WIDTH; x++ ) {
      
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
//Intersection ClosestIntersection(Ray ray){
//  Intersection closestIntersection;
//  
//  int i = 0;
//  for (vector<Object*>::iterator itr = Objects.begin(); itr < Objects.end(); itr++) {
//    Intersection intersection = (*itr)->intersect(ray, i);
//    
//    if(i != ray.objectIndex && intersection.distance < closestIntersection.distance)  {
//      closestIntersection = intersection;
//    }
//    
//    //    if (intersection.didIntersect && intersection.distance < closestIntersection.distance) {
//    //      closestIntersection = intersection;
//    //    }
//    
//    i++;
//    
//  }
//  
//  return closestIntersection;
//}

Intersection ClosestIntersection(Ray ray){
  Intersection closestIntersection;
  closestIntersection.didIntersect = false;
  closestIntersection.distance = 50000.00;
  
  //Extract the triangle information from the mesh, and draw the triangle
  int i = 0;
  for (vector<Object*>::iterator itr = Objects.begin(); itr < Objects.end(); itr++) {
    vector<Triangle> _triangles = (*itr)->triangles;
    for (int j = 0; j < _triangles.size(); ++j) {
      //      intersection a = object.intersection;
      vec3 x =  closestIntersection.Calculate_Intersection(_triangles[j], ray.start, ray.dir);
      if(i != ray.objectIndex && closestIntersection.Intersects(x))  {
        // If the current intersection is closer than previous, update
        if(closestIntersection.distance > x.x) {
          closestIntersection.distance = x.x;
          closestIntersection.position = ray.start + x.x * ray.dir;
          closestIntersection.triangleIndex = make_pair(i,j);
          closestIntersection.didIntersect = true;
        }
      }
    }
    i++;
  }

  
  return closestIntersection;
}

vec3 DirectLight( const Intersection& intersection ){
  vec3 light(0.0f,0.0f,0.0f);
  vec3 positions[100];
  SoftShadowPositions(positions);
  int objectIndex = intersection.triangleIndex.first;
  int triangleIndex = intersection.triangleIndex.second;
  
  for (int k = 0; k < control.SOFT_SHADOWS_SAMPLES; k++) {
    // Unit vector from point of intersection to light
    vec3 surfaceToLight = glm::normalize(positions[k] - intersection.position);
    // Distance from point of intersection to light
    float radius = glm::length(positions[k] - intersection.position);
    
    Triangle triangle = Objects[objectIndex]->triangles[triangleIndex];
    // Unit vector perpendicular to plane.
    vec3 normal = glm::normalize(triangle.normal);
    // Direct light intensity given distance/radius
    float lightIntensity = max( glm::dot(normal, surfaceToLight) , 0 ) / (4 * M_PI * radius * radius);
    
    
    Ray ray = Ray(positions[k], -surfaceToLight, NULLobjectIndex);
    
    Intersection nearestTriangle = ClosestIntersection(ray);
    
    int nearestTriangleObjectIndex = nearestTriangle.triangleIndex.first;
    int nearestTriangleTriangleIndex = nearestTriangle.triangleIndex.second;
    if (objectIndex != nearestTriangleObjectIndex || triangleIndex != nearestTriangleTriangleIndex){
      // If intersection is closer to light source than self
      if (nearestTriangle.distance < radius * 0.99f)
        lightIntensity = 0; // Zero light intensity
    }
    
    light += ( Lights[0].color / (float) control.SOFT_SHADOWS_SAMPLES ) * lightIntensity;
  }
  return light + indirectLight;
}

void SoftShadowPositions(vec3 positions[]){
  // Set first ray projection to be at 'light position'
  positions[0] = Lights[0].position;
  // Settings to handle variable number of soft shadows
  float mul3 = (float) (control.SOFT_SHADOWS_SAMPLES - 1) / 3;
  float shift = (mul3 < 1) ? 0.007 : (mul3 < 4) ? 0.005 : (mul3 < 6) ? 0.004 : (mul3 < 11) ? 0.003 : 0.002 ;
  
  // Find equal positions around light source
  for(int i = 1; i < control.SOFT_SHADOWS_SAMPLES; i++) {
    float sign  = (i % 6 >= 3) ? -1 : 1;            // Rays from +/-
    int mod = i % 3;                                // Rays from (x,y,z)
    if (mod == 0){
      positions[i] = Lights[0].position + vec3(shift, 0, 0) * sign * (float) (i + 1.0f); // Shift in X
    }
    else if(mod == 1){
      positions[i] = Lights[0].position + vec3(0, shift, 0) * sign * (float) i;          // Shift in Y
    }
    else {
      positions[i] = Lights[0].position + vec3(0, 0, shift) * sign * (float) (i - 1.0f); // Shift in Z
    }
  }
}

void AASampling(int pixelx, int pixely) {
  vec3 average_color = traceRayFromCamera(pixelx, pixely);
  
  bool resample = false;
  float pixel_distance = 1;    // -1 <= x/y <= 1
  float steps = 1;             // Step between -1 and 1 in steps of 1/1.
  
  for( float k = - pixel_distance; ( k <= pixel_distance && !resample ) ; k += (pixel_distance / steps)){
    for( float m = - pixel_distance; ( m <= pixel_distance && !resample ) ; m += (pixel_distance / steps)){
      
      vec3 current_color = traceRayFromCamera(pixelx + k, pixely + m);
      
      // Edge dectection using current and previous aliasing points colour difference.  Resample at these points with more accuracy
      if ( EdgeDectection(current_color, average_color) ){
        resample = true;
      }
      else if (k > - pixel_distance +  pixel_distance / steps) {                // IF SECOND ITERATION OF K OCCURS
        screenPixels[pixelx][pixely] = (average_color + current_color) / 2.0f;
        return;
      }
      else{
        average_color = ( average_color + current_color ) / 2.0f;
      }
      
    }
  }
  
  if (control.SHOW_EDGES){
    average_color = (resample) ? vec3(0,0,0) : average_color;
  } else {
    average_color = (resample) ? AASuperSampling(pixelx, pixely) : average_color;
  }
  
  screenPixels[pixelx][pixely] = average_color;
}

// Sample around current edge point and take average.
vec3 AASuperSampling(float pixelx, float pixely){
  vec3 average_color = traceRayFromCamera(pixelx, pixely);
  
  float pixel_distance = 1;    // -1 <= x/y <= 1
  float steps = 2;             // Step between -1 and 1 in steps of 1/2.
  
  for( float k = - pixel_distance; ( k <= pixel_distance ) ; k += (pixel_distance / steps)){
    for( float m = - pixel_distance; ( m <= pixel_distance ) ; m += (pixel_distance / steps)){
      
      average_color = ( average_color + traceRayFromCamera(pixelx + k, pixely + m) ) / 2.0f;
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








float getReflectance(const vec3 normal, const vec3 incident, float n1, float n2) {
  float n = n1 / n2;
  float cosI = dot(-normal,incident);
  float sinT2 = n * n * (1.0 - cosI * cosI);
  
  if (sinT2 > 1.0) {
    // Total Internal Reflection.
    return 1.0;
  }
  
  float cosT = sqrt(1.0 - sinT2);
  float r0rth = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
  float rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
  return (r0rth * r0rth + rPar * rPar) / 2.0;
}

vec3 refractVector(const vec3 normal, const vec3 incident, float n1, float n2) {
  float n = n1 / n2;
  float cosI = dot(-normal,incident);
  float sinT2 = n * n * (1.0 - cosI * cosI);
  
  if (sinT2 > 1.0) {
    cerr << "Bad refraction vector!" << endl;
    exit(EXIT_FAILURE);
  }
  
  float cosT = sqrt(1.0 - sinT2);
  return incident * n + normal * (n * cosI - cosT);
}


vec3 reflectVector(vec3 vector, vec3 normal) {
  return normal * 2.0f * dot(vector,normal) - vector;
}

vec3 getAmbientLighting(Intersection intersection) {
  return DirectLight(intersection) * Objects[intersection.triangleIndex.first]->triangles[intersection.triangleIndex.second].color;
}

vec3 getReflectiveRefractiveLighting(const Intersection& intersection, Ray ray, int depth) {
  
  float reflectivity = Objects[intersection.triangleIndex.first]->material.getReflectivity();
  //  float startRefractiveIndex = intersection.startMaterial->getRefractiveIndex();
  float startRefractiveIndex = 0.9f;
  float endRefractiveIndex = Objects[intersection.triangleIndex.first]->material.getRefractiveIndex();
  
  int reflectionsRemaining = 1;
  
  vec3 normal = Objects[intersection.triangleIndex.first]->triangles[intersection.triangleIndex.second].normal;
  
  /**
   * Don't perform lighting if the object is not reflective or refractive or we have
   * hit our recursion limit.
   */
  if ((reflectivity == 0.0f && endRefractiveIndex == 0.0f) || reflectionsRemaining <= 0) {
    return vec3(0.0f,0.0f,0.0f);
  }
  
  // Default to exclusively reflective values.
  float reflectivePercentage = reflectivity;
  float refractivePercentage = 0;
  
  
  // Refractive index overrides the reflective property.
  if (endRefractiveIndex != 0.0f) {
    reflectivePercentage = getReflectance(normal, ray.dir, startRefractiveIndex, endRefractiveIndex);
    
    refractivePercentage = 1 - reflectivePercentage;
  }
  
  // No ref{ra,le}ctive properties - bail early.
  if (refractivePercentage <= 0 && reflectivePercentage <= 0) {
    return vec3(0.0f,0.0f,0.0f);
  }
  
  vec3 reflectiveColor;
  vec3 refractiveColor;
  
  if (reflectivePercentage > 0) {
    vec3 reflected = reflectVector(ray.start,
                                   normal);
    Ray reflectedRay(intersection.position, reflected, intersection.triangleIndex.first);
    
    
    reflectiveColor = getColor(reflectedRay, depth + 1) * reflectivePercentage;
  }
  
  if (refractivePercentage > 0) {
    vec3 refracted = refractVector(normal, ray.dir, startRefractiveIndex, endRefractiveIndex);
    Ray refractedRay = Ray(intersection.position, refracted, intersection.triangleIndex.first);
    
    refractiveColor = getColor(refractedRay, depth + 1) * refractivePercentage;
  }
  
  return reflectiveColor + refractiveColor;
}

vec3 getColor(Ray ray, int depth){
  
  if(depth > MAX_DEPTH) return vec3(2,0,0);
  
  Intersection closestIntersection = ClosestIntersection(ray);
  vec3 color = vec3(0,0,0);
  
  //Fill a pixel with the color of the closest triangle intersecting the ray, black otherwise
  if(closestIntersection.didIntersect) {
    vec3 ambientColor = getAmbientLighting(closestIntersection);
    vec3 reflectedColor = getReflectiveRefractiveLighting(closestIntersection, ray, depth);
    return ambientColor + reflectedColor;
  }
  // No Intersection
  else {
    color = vec3(0,0,0);
  }
  
  return color;
}

vec3 traceRayFromCamera(float x , float y) {
  // Get direction of camera
  vec3 direction(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, FOCAL_LENGTH);
  direction = direction * camera.rotation;
  Ray ray = Ray(camera.position, direction, NULLobjectIndex);
  
  
  return getColor(ray, 0);
}


void Calculate_DOF() {
  
  int DOF_KERNEL_SIZE = 8;
  float totalPixels = DOF_KERNEL_SIZE * DOF_KERNEL_SIZE;
  int trim = DOF_KERNEL_SIZE/2;
  
  for( int y=trim; y<SCREEN_HEIGHT-trim; y++ ) {
    for( int x=trim; x<SCREEN_WIDTH-trim; x++ ) {
      
      vec3 average_color = vec3( 0,0,0 );
      
      vec3 direction(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, FOCAL_LENGTH);
      direction = direction * camera.rotation;
      Ray ray = Ray(camera.position, direction, NULLobjectIndex);
      
      Intersection closestIntersection_xy = ClosestIntersection(ray);
      
      
      // Start from top left of kernel
      for(int y1 = ceil(- DOF_KERNEL_SIZE / 2.0f); y1 < ceil(DOF_KERNEL_SIZE / 2.0f); y1++)
      {
        for(int x1 = ceil(- DOF_KERNEL_SIZE / 2.0f); x1 < ceil(DOF_KERNEL_SIZE / 2.0f); x1++)
        {
          float weighting = 1.0f / totalPixels;
          
          //      float distance_metric = abs(closestIntersection_xy.distance * 100000 - FOCAL_LENGTH);
          
          float distance_metric = abs(closestIntersection_xy.distance * 30 * control.DOF_VALUE);
          
          if(y1 == 0 && x1 == 0)
            weighting = 1 - (std::min(distance_metric, 1.0f) * ((totalPixels - 1) / totalPixels) );
          else
            weighting =      std::min(distance_metric, 1.0f) * (1.0f / totalPixels);
          
          // Add contribution to final pixel colour
          average_color += screenPixels[x+x1][y+y1] * weighting;
        }
      }
      
      screenPixels[x][y] = average_color;
      
    }
  }
}


vec3 traceDofFromCamera1(float x, float y) {
  vec3 average_color = vec3( 0,0,0 );
  vec3 lightIntensity = vec3( 0,0,0 );
  
  int DOF_KERNEL_SIZE = 2;
  
  float totalPixels = DOF_KERNEL_SIZE * DOF_KERNEL_SIZE;
  
  
  for (int i = 0; i < 4; i++){
    
    
    // Get direction of camera
    vec3 direction(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, FOCAL_LENGTH);
    direction = direction * camera.rotation;
    
    float shift = 0.001 * control.DOF_VALUE;
    
    vec3 local_cameraPos = vec3(camera.position.x + shift, camera.position.y, camera.position.z);
    
    if (i == 1){
      local_cameraPos = vec3(camera.position.x - shift, camera.position.y, camera.position.z);
    }
    else if (i == 2){
      local_cameraPos = vec3(camera.position.x, camera.position.y + shift, camera.position.z);
    }
    else if (i == 3) {
      local_cameraPos = vec3(camera.position.x, camera.position.y - shift, camera.position.z);
    }
    
    Ray ray = Ray(local_cameraPos, direction, NULLobjectIndex);
    
    Intersection closestIntersection = ClosestIntersection(ray);
    
    
    // If ray casting from camera position hits a triangle
    if(closestIntersection.didIntersect) {
      // Identify triangle, find colour and 'Put' corrisponding pixel
      int objectindex = closestIntersection.triangleIndex.first;
      int triangleindex = closestIntersection.triangleIndex.second;
      vec3 color = Objects[objectindex]->triangles[triangleindex].color;
      lightIntensity = DirectLight(closestIntersection);
      float weighting = 1 - (std::min(abs(closestIntersection.distance), 1.0f) * ((totalPixels - 1) / totalPixels) );
      average_color += color * weighting;
    }
    else {
      return vec3( 0,0,0 );
    }
  }
  return average_color * lightIntensity / 4.0f;
}

vec3 AddVectorAndAverage(vec3 A, vec3 B) {
  return vec3( A.x + B.x ,  A.y + B.y , A.z + B.z ) / 2.0f;
}



