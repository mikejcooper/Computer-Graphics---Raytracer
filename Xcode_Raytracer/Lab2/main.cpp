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
//    LoadGenericmodel(&Objects);
  LoadTestModel( &Objects );
  kdtree = new KDTree(0, 'z', Objects);
  
  

  
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
    if(control.TAKEPICTURE){
      SDL_SaveBMP( screen, "screenshot.bmp" );
      return 0;
    }
  }
  
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


vec3 traceRayFromCamera(float x , float y) {
  // Get direction of camera
  vec3 direction(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, FOCAL_LENGTH);
  direction = direction * camera.rotation;
  Ray ray = Ray(camera.position, direction, NULLobjectIndex);
  return getColor(ray, 0);
}


vec3  indirectLight = 0.2f * vec3( 1, 1, 1 );



void createCoordinateSystem(const vec3 &N, vec3 &Nt, vec3 &Nb)
{
  if (std::fabs(N.x) > std::fabs(N.y))
    Nt = vec3(N.z, 0, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
  else
    Nt = vec3(0, -N.z, N.y) / sqrtf(N.y * N.y + N.z * N.z);
  Nb = cross(N,Nt);
}

vec3 uniformSampleHemisphere(const float &r1, const float &r2)
{
  // cos(theta) = u1 = y
  // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
  float sinTheta = sqrtf(1 - r1 * r1);
  float phi = 2 * M_PI * r2;
  float x = sinTheta * cosf(phi);
  float z = sinTheta * sinf(phi);
  return vec3(x, r1, z);
}

vec3 IndirectLight(Intersection intersection, Ray ray, int depth){
  if(!control.GLOBALILLUMINATION)
    return indirectLight;
    
  vec3 _indirectLight = vec3(0,0,0);
  
  //  Create position bias to prevent self reflection
  float bias_tmp = 0.1f;
  vec3 bias = bias_tmp * intersection.normal;
  bool outside = dot(ray.dir,intersection.normal) < 0;
  vec3 intersectPosition = outside ? intersection.position - bias : intersection.position + bias;


  float N =4;// / (depth + 1);
  vec3 Nt; // Perpendicular to Normal
  vec3 Nb; // Perpendicular to Normal and Nt
  // Compute shaded point coordinate system using normal N. Co-System: N, Nt, Nb.
  createCoordinateSystem(intersection.normal, Nt, Nb);
  float pdf = 1 / (2 * M_PI); // PDF in this case is constant, because all rays or all directions have the same probability of being generated
  for (int n = 0; n < N; ++n) {
    float r1 = ((float) rand() / (RAND_MAX));
    float r2 = ((float) rand() / (RAND_MAX));
    vec3 sample = uniformSampleHemisphere(r1, r2);
    vec3 sampleWorld(
                      sample.x * Nb.x + sample.y * intersection.normal.x + sample.z * Nt.x,
                      sample.x * Nb.y + sample.y * intersection.normal.y + sample.z * Nt.y,
                      sample.x * Nb.z + sample.y * intersection.normal.z + sample.z * Nt.z);
    // don't forget to divide by PDF and multiply by cos(theta)
//    vec3 randomDirection = getRandomDirectionHemisphere(N);
    Ray angleRay = Ray(intersectPosition + sampleWorld, sampleWorld, intersection.objIndex);
    vec3 illumination = getColor(angleRay, depth + 1);
    _indirectLight += r1 * illumination / pdf; // Mathematically we also need to divide this result by the probability of generating the direction Dr.
  }
  return (_indirectLight / N);
}

vec3 getColor(Ray ray, int depth){
  
  if(depth > MAX_DEPTH) return vec3(0,0,0);
  
//  float dispersion = 5.0f;
//  vec3 disturbance((dispersion / RAND_MAX) * (1.0f * rand()), (dispersion / RAND_MAX) * (1.0f * rand()), 0.0f);
  
  Intersection closestIntersection = ClosestIntersection(ray);
  
  //Fill a pixel with the color of the closest triangle intersecting the ray, black otherwise
  if(closestIntersection.didIntersect) {
    vec3 color = Objects[closestIntersection.objIndex]->material.getColor();
    vec3 _indirectLight = IndirectLight(closestIntersection, ray, depth);
    vec3 _directLight = DirectLight(closestIntersection);
    vec3 reflectedColor = getReflectiveRefractiveLighting(closestIntersection, ray, depth);
//    if(Objects[closestIntersection.objIndex]->material.getId() == 3){
//      
//      return directLight*0.4f + reflectedColor;
//    }
    return color * (_directLight + _indirectLight) + reflectedColor;
  }
  // No Intersection
  else {
    return vec3(0,0,0);
  }
}


/*
 ClosestIntersection takes the start position of the ray and its direction and a std::vector of Triangles
 as input. It should then check for intersection against all these Triangles. If an
 intersection occurred it should return true. Otherwise false. In the case of an intersection
 it should also return some information about the closest intersection.
 */
Intersection ClosestIntersection(Ray ray){
  if(control.KDTREE)
    return kdtree->getClosestIntersection(ray);
  
  Intersection closestIntersection = Intersection();
  
  for (vector<Object*>::iterator itr = Objects.begin(); itr < Objects.end(); itr++) {
      
    Intersection intersection = (*itr)->intersect(ray, int (itr - Objects.begin()));
    
    if(intersection.didIntersect && intersection.distance < closestIntersection.distance)  {
      closestIntersection = intersection;
    }
    
  }
  return closestIntersection;
}

vec3 getSpecularLighting(const Intersection& intersection, Light* light, const Ray& ray) {
  
  vec3 specularColor(0.0, 0.0, 0.0);
  float shininess = Objects[intersection.objIndex]->material.getShininess();
  
  if (shininess == 0.0f) {
    /* Don't perform specular lighting on non shiny objects. */
    return specularColor;
  }
  
  vec3 view = normalize(ray.start - intersection.position);
  vec3 lightOffset = light->position - intersection.position;
  
  vec3 reflected = intersection.normal * 2.0f * glm::dot(normalize(lightOffset),intersection.normal) - normalize(lightOffset);
  
  float dot = glm::dot(view,reflected);
  
  if (dot <= 0) {
    return specularColor;
  }
  
  vec3 specularAmount = pow(dot, shininess) * light->color * 0.005f;
    
  return specularAmount;
}



vec3 DirectLight( const Intersection& intersection ){
  vec3 diffuseColor(0.0f,0.0f,0.0f);
  vec3 specularColor(0.0f,0.0f,0.0f);
  vec3 light(0.0f,0.0f,0.0f);
  vec3 positions[100];
  SoftShadowPositions(positions);
  
  for (int k = 0; k < control.SOFT_SHADOWS_SAMPLES; k++) {
    // Unit vector from point of intersection to light
    vec3 surfaceToLight = glm::normalize(positions[k] - intersection.position);
    // Distance from point of intersection to light
    float radius = glm::length(positions[k] - intersection.position);
    
    // Unit vector perpendicular to plane.
    vec3 normal = glm::normalize(intersection.normal);
    // Direct light intensity given distance/radius
    float lightIntensity = max( glm::dot(normal, surfaceToLight) , 0 ) / (4 * M_PI * radius * radius);
    
    Ray ray = Ray(positions[k], -surfaceToLight, intersection.objIndex);
    Intersection closestIntersection = ClosestIntersection(ray);
    
    if (intersection.objIndex != closestIntersection.objIndex){
      // If intersection is closer to light source than self
      if (closestIntersection.distance < radius * 0.99f){
        float refractiveIndex = Objects[closestIntersection.objIndex]->material.getRefractiveIndex();
        float shadowAlpha = (refractiveIndex == 1) ? 0.0f : 1.0f / 90.0f * refractiveIndex;
        lightIntensity = shadowAlpha; // Zero light intensity
//        lightIntensity = 0.0f;
      }
    }
    
    diffuseColor = (diffuseColor + lightIntensity) * ( Lights[0].color / (float) control.SOFT_SHADOWS_SAMPLES );
    specularColor = specularColor + getSpecularLighting(intersection, &Lights[0], ray);
    
//    light += ( Lights[0].color / (float) control.SOFT_SHADOWS_SAMPLES ) * lightIntensity;
  }
  return specularColor + diffuseColor;
}




#define clamp(x, upper, lower) (fmin(upper, fmax(x, lower)))

float fresnel(const vec3 I, const vec3 N, const float ior)
{
  float kr;
  float cosi = clamp(-1, 1, dot(I, N));
  float etai = 1, etat = ior;
  if (cosi > 0) { std::swap(etai, etat); }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1) {
    kr = 1;
  }
  else {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi = fabsf(cosi);
    float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    kr = (Rs * Rs + Rp * Rp) / 2;
  }
  return kr;
  // As a consequence of the conservation of energy, transmittance is given by:
  // kt = 1 - kr;
}

vec3 refract(const vec3 I, const vec3 N, const float ior)
{
  float cosi = clamp(-1, 1, dot(I,N));
  float etai = 1, etat = ior;
  vec3 n = N;
  if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
  float eta = etai / etat;
  float k = 1 - eta * eta * (1 - cosi * cosi);
  return k < 0 ? vec3(0,0,0) : eta * I + (eta * cosi - sqrtf(k)) * n;
}

vec3 reflectVector(vec3 vector, vec3 normal) {
  return normal * 2.0f * dot(vector,normal) - vector;
}

vec3 getReflectiveRefractiveLighting(const Intersection& intersection, Ray ray, int depth) {
  vec3 reflectiveColor;
  vec3 refractiveColor;
    
  float reflectivity = Objects[intersection.objIndex]->material.getReflectivity();
  float refractiveIndex = Objects[intersection.objIndex]->material.getRefractiveIndex();
  
  float bias_tmp = 0.1f;
  float kr = fresnel(ray.dir, intersection.normal, refractiveIndex);
  bool outside = dot(ray.dir,intersection.normal) < 0;
  vec3 bias = bias_tmp * intersection.normal;
  
  // compute refraction if it is not a case of total internal reflection
  if (kr < 1 && refractiveIndex > 1)
  {
    vec3 refractionDirection = normalize(refract(ray.dir, intersection.normal, refractiveIndex));
    vec3 refractionRayOrig = outside ? intersection.position - bias : intersection.position + bias;
    Ray refractRay = Ray(refractionRayOrig, refractionDirection, intersection.objIndex);
    refractiveColor = getColor(refractRay, depth+1);
  }
  
  if(reflectivity > 0)
  {
    vec3 reflected = reflectVector(ray.start, intersection.normal);
    vec3 reflectionRayOrig = outside ? intersection.position + bias : intersection.position - bias;
    Ray reflectedRay(reflectionRayOrig, reflected, intersection.objIndex);
    reflectiveColor = getColor(reflectedRay, depth + 1);
    return reflectiveColor * reflectivity + refractiveColor * (1 - kr);
  }
  // mix the two
  return  refractiveColor * (1 - kr);
  
  
//  if (reflectivity > 0) {
//    vec3 reflected = reflectVector(ray.start, intersection.normal);
//    Ray reflectedRay(intersection.position, reflected, intersection.objIndex);
//    reflectiveColor = getColor(reflectedRay, depth + 1);
//  }
//  
//  return reflectiveColor + refractiveColor;
}







// -------------------------- EXTRAS --------------------------------------------


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
      int objectindex = closestIntersection.objIndex;
      vec3 color = Objects[objectindex]->material.getColor();
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




