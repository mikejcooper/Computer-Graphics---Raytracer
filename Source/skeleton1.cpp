#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


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

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#define OCLFILE         "Source/kernels.cl"


/* struct to hold OpenCL objects */
typedef struct
{
  cl_device_id      device;
  cl_context        context;
  cl_command_queue  queue;

  cl_program program;
  cl_kernel  accelerate_flow;

  cl_mem cells;
  cl_mem tmp_cells;
  cl_mem obstacles;
} t_ocl;


int timestep(t_ocl ocl);
int accelerate_flow(t_ocl ocl);




int initialise(t_ocl* ocl);
int finalise(t_ocl ocl);

/* utility functions */
void checkError(cl_int err, const char *op, const int line);
void die(const char* message, const int line, const char* file);
void usage(const char* exe);

cl_device_id selectOpenCLDevice();

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
mat3  rotationLeft(cos(turnAngle),0,-sin(turnAngle),0,1,0,sin(turnAngle),0,cos(turnAngle));
mat3  rotationRight(cos(-turnAngle),0,-sin(-turnAngle),0,1,0,sin(-turnAngle),0,cos(-turnAngle));
SDL_Surface* screen;
int t;
vector<Triangle> triangles;


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS   */

void    Control_LightSource(Uint8* keystate);
void    Control_Camera(Uint8* keystate);
vec3    Get_Vector_AtoB(vec3 a,vec3 b);
float   Max_Value(float a,float b);
vec3    DirectLight( const Intersection& intersection );
mat3    Cramers_Inverse(mat3 A);
void    Draw_Triangle_Hit(Intersection& closestIntersection,int x,int y);
vec3    Cramers_Calculate_Column(mat3 A,int identity_column);
double  Cramers_Get_Determinant_Variable(mat3 A,int replace_column,int identity_column);
double  Get_Matrix_Determinant(mat3 A);
void    Print_Matrix(mat3 A);
bool    Does_Vector_Intersect_Triangle(vec3 x);
vec3    Get_TUV_Values(Triangle triangle,vec3 start,vec3 dir);
void    Control();
void    Update();
void    Draw();
bool    ClosestIntersection(vec3 start, vec3 dir, Intersection& closestIntersection );
mat3    Cramers_Inverse(mat3 A);

/*
** main program:
*/
int main(int argc, char* argv[])
{
  struct timeval timstr;        /* structure to hold elapsed time */
  struct rusage ru;             /* structure to hold CPU time--system and user */
  double tic, toc;              /* floating point numbers to calculate elapsed wallclock time */
  double usrtim;                /* floating point number to record elapsed user CPU time */
  double systim;                /* floating point number to record elapsed system CPU time */
  t_ocl ocl;


  /* initialise our data structures and load values from file */
  initialise(&ocl);
  LoadTestModel( triangles );
  
  // printf("%f\n", triangles[0].T1.v0.x);
  // for(int i = 0; i < triangles.size(); i++){
  //   printf("%f\n", triangles[i].v0.x);

  // }

  screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
  t = SDL_GetTicks(); 


  /* iterate for maxIters timesteps */
  gettimeofday(&timstr, NULL);
  tic = timstr.tv_sec + (timstr.tv_usec / 1000000.0);

    while( NoQuitMessageSDL() )
  {
    Update();
    Draw();
  }

  SDL_SaveBMP( screen, "screenshot.bmp" );


  // for loop here   
  timestep(ocl);


  gettimeofday(&timstr, NULL);
  toc = timstr.tv_sec + (timstr.tv_usec / 1000000.0);
  getrusage(RUSAGE_SELF, &ru);
  timstr = ru.ru_utime;
  usrtim = timstr.tv_sec + (timstr.tv_usec / 1000000.0);
  timstr = ru.ru_stime;
  systim = timstr.tv_sec + (timstr.tv_usec / 1000000.0);

  /* write final values and free memory */
  printf("==done==\n");
  printf("Elapsed time:\t\t\t%.6lf (s)\n", toc - tic);
  printf("Elapsed user CPU time:\t\t%.6lf (s)\n", usrtim);
  printf("Elapsed system CPU time:\t%.6lf (s)\n", systim);
  // write_values(params, cells, obstacles, av_vels);
  finalise(ocl);

  return EXIT_SUCCESS;
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


int timestep(t_ocl ocl)
{
  cl_int err;
  int temp[100] = {0,1,2};


  
  glm::vec3 v0;
  v0.x = 5;

  // size_t row_size = A[0].size() * sizeof(A[0][0]);

  // for(size_t row = 0; row < A.size(); ++row)
  //     queue.enqueueWriteBuffer(A_d, CL_TRUE, /*offset=*/row * row_size, /*size=*/row_size, &A[row][0]);

  // Write cells to OpenCL buffer
  err = clEnqueueWriteBuffer(
    ocl.queue, ocl.cells, CL_TRUE, 0,
    sizeof(vec3) * 3, &v0, 0, NULL, NULL);
  checkError(err, "writing cells data", __LINE__);

    v0.x = 5;


  accelerate_flow(ocl);


  // Read cells from device
  err = clEnqueueReadBuffer(
    ocl.queue, ocl.cells, CL_TRUE, 0,
    sizeof(vec3) * 3, &v0, 0, NULL, NULL);
  checkError(err, "reading tmp_cells data", __LINE__);

  printf("%f\n", v0.x);

  // collision(params, cells, tmp_cells, obstacles, ocl);
  return EXIT_SUCCESS;
}

int accelerate_flow(t_ocl ocl)
{
  cl_int err;
  int test = 4;

  // Set kernel arguments
  err = clSetKernelArg(ocl.accelerate_flow, 0, sizeof(cl_mem), &ocl.cells);
  checkError(err, "setting accelerate_flow arg 0", __LINE__);
  err = clSetKernelArg(ocl.accelerate_flow, 1, sizeof(cl_int), &test);
  checkError(err, "setting accelerate_flow arg 1", __LINE__);
  

  // Enqueue kernel
  size_t global[1] = {test};
  // size_t global[2] = {params.nx, params.ny};
  // size_t local[2]  = {params.nx, 1};

  err = clEnqueueNDRangeKernel(ocl.queue, ocl.accelerate_flow,
                               1, NULL, global, NULL, 0, NULL, NULL);
  checkError(err, "enqueueing accelerate_flow kernel", __LINE__);

  // Wait for kernel to finish
  err = clFinish(ocl.queue);
  checkError(err, "waiting for accelerate_flow kernel", __LINE__);

  return EXIT_SUCCESS;
}

int initialise(t_ocl *ocl)
{
  char   message[1024];  /* message buffer */
  FILE*   fp;            /* file pointer */
  char*  ocl_src;        /* OpenCL kernel source */
  long   ocl_size;       /* size of OpenCL kernel source */

  cl_int err;

  ocl->device = selectOpenCLDevice();

  // Create OpenCL context
  ocl->context = clCreateContext(NULL, 1, &ocl->device, NULL, NULL, &err);
  checkError(err, "creating context", __LINE__);

  fp = fopen(OCLFILE, "r");
  if (fp == NULL)
  {
    perror("Error");
    sprintf(message, "could not open OpenCL kernel file: %s", OCLFILE);
    die(message, __LINE__, __FILE__);
  }

  // Create OpenCL command queue
  ocl->queue = clCreateCommandQueue(ocl->context, ocl->device, 0, &err);
  checkError(err, "creating command queue", __LINE__);

  // Load OpenCL kernel source
  fseek(fp, 0, SEEK_END);
  ocl_size = ftell(fp) + 1;
  ocl_src = (char*)malloc(ocl_size);
  memset(ocl_src, 0, ocl_size);
  fseek(fp, 0, SEEK_SET);
  fread(ocl_src, 1, ocl_size, fp);
  fclose(fp);

  // Create OpenCL program
  ocl->program = clCreateProgramWithSource(
    ocl->context, 1, (const char**)&ocl_src, NULL, &err);
  free(ocl_src);
  checkError(err, "creating program", __LINE__);

  // Build OpenCL program
  err = clBuildProgram(ocl->program, 1, &ocl->device, "", NULL, NULL);
  if (err == CL_BUILD_PROGRAM_FAILURE)
  {
    size_t sz;
    clGetProgramBuildInfo(
      ocl->program, ocl->device,
      CL_PROGRAM_BUILD_LOG, 0, NULL, &sz);
    char *buildlog = new char[1024];
    clGetProgramBuildInfo(
      ocl->program, ocl->device,
      CL_PROGRAM_BUILD_LOG, sz, buildlog, NULL);
    fprintf(stderr, "\nOpenCL build log:\n\n%s\n", buildlog);
    free(buildlog);
  }
  checkError(err, "building program", __LINE__);

  // Create OpenCL kernels
  ocl->accelerate_flow = clCreateKernel(ocl->program, "accelerate_flow", &err);
  checkError(err, "creating accelerate_flow kernel", __LINE__);

  // Allocate OpenCL buffers
  ocl->cells = clCreateBuffer(
    ocl->context, CL_MEM_READ_WRITE,
    sizeof(cl_int) * 100, NULL, &err);
  checkError(err, "creating cells buffer", __LINE__);


  return EXIT_SUCCESS;
}

int finalise( t_ocl ocl)
{

  clReleaseMemObject(ocl.cells);

  clReleaseKernel(ocl.accelerate_flow);
  clReleaseProgram(ocl.program);
  clReleaseCommandQueue(ocl.queue);
  clReleaseContext(ocl.context);

  return EXIT_SUCCESS;
}



void checkError(cl_int err, const char *op, const int line)
{
  if (err != CL_SUCCESS)
  {
    fprintf(stderr, "OpenCL error during '%s' on line %d: %d\n", op, line, err);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
}

void die(const char* message, const int line, const char* file)
{
  fprintf(stderr, "Error at line %d of file %s:\n", line, file);
  fprintf(stderr, "%s\n", message);
  fflush(stderr);
  exit(EXIT_FAILURE);
}

void usage(const char* exe)
{
  fprintf(stderr, "Usage: %s <paramfile> <obstaclefile>\n", exe);
  exit(EXIT_FAILURE);
}

#define MAX_DEVICES 32
#define MAX_DEVICE_NAME 1024

cl_device_id selectOpenCLDevice()
{
  cl_int err;
  cl_uint num_platforms = 0;
  cl_uint total_devices = 0;
  cl_platform_id platforms[8];
  cl_device_id devices[MAX_DEVICES];
  char name[MAX_DEVICE_NAME];

  // Get list of platforms
  err = clGetPlatformIDs(8, platforms, &num_platforms);
  checkError(err, "getting platforms", __LINE__);

  // Get list of devices
  for (cl_uint p = 0; p < num_platforms; p++)
  {
    cl_uint num_devices = 0;
    err = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL,
                         MAX_DEVICES-total_devices, devices+total_devices,
                         &num_devices);
    checkError(err, "getting device name", __LINE__);
    total_devices += num_devices;
  }

  // Print list of devices
  printf("\nAvailable OpenCL devices:\n");
  for (cl_uint d = 0; d < total_devices; d++)
  {
    clGetDeviceInfo(devices[d], CL_DEVICE_NAME, MAX_DEVICE_NAME, name, NULL);
    printf("%2d: %s\n", d, name);
  }
  printf("\n");

  // Use first device unless OCL_DEVICE environment variable used
  cl_uint device_index = 0;
  char *dev_env = getenv("OCL_DEVICE");
  if (dev_env)
  {
    char *end;
    device_index = strtol(dev_env, &end, 10);
    if (strlen(end))
      die("invalid OCL_DEVICE variable", __LINE__, __FILE__);
  }

  if (device_index >= total_devices)
  {
    fprintf(stderr, "device index set to %d but only %d devices available\n",
            device_index, total_devices);
    exit(1);
  }

  // Print OpenCL device name
  clGetDeviceInfo(devices[device_index], CL_DEVICE_NAME,
                  MAX_DEVICE_NAME, name, NULL);
  printf("Selected OpenCL device:\n-> %s (index=%d)\n\n", name, device_index);

  return devices[device_index];
}
