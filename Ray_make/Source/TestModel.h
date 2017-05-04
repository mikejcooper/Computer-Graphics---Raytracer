#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box
using namespace std;
using namespace glm;
#include "glm/glm.hpp"
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include "Material.hpp"
#include "Materials.hpp"
#include "Triangle.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"



void LoadTestModel( std::vector<Cube*> *Objects )
{
  std::vector<Triangle> triangles;
  
  using glm::vec3;
  
  // Defines colors:
  vec3 red(    0.75f, 0.15f, 0.15f );
  vec3 yellow( 0.75f, 0.75f, 0.15f );
  vec3 green(  0.15f, 0.75f, 0.15f );
  vec3 cyan(   0.15f, 0.75f, 0.75f );
  vec3 blue(   0.15f, 0.15f, 0.75f );
  vec3 purple( 0.75f, 0.15f, 0.75f );
  vec3 white(  0.75f, 0.75f, 0.75f );
  vec3 trans(0.0f,0.0f,0.0f);
  
  triangles.clear();
  triangles.reserve( 5*2*3 );
  
  // ---------------------------------------------------------------------------
  // Room
  
  float L = 555;			// Length of Cornell Box side.
  
  vec3 A(L,0,0);
  vec3 B(0,0,0);
  vec3 C(L,0,L);
  vec3 D(0,0,L);
  
  vec3 E(L,L,0);
  vec3 F(0,L,0);
  vec3 G(L,L,L);
  vec3 H(0,L,L);
  
  // Floor:
  triangles.push_back( Triangle( C, B, A, green ) );
  triangles.push_back( Triangle( C, D, B, green ) );
  
  Objects->push_back(new Cube(triangles, Diffuse(green)));
  triangles.clear();
  
  // Left wall
  triangles.push_back( Triangle( A, E, C, purple ) );
  triangles.push_back( Triangle( C, E, G, purple ) );
  
  Objects->push_back(new Cube(triangles, Diffuse(purple)));
  triangles.clear();
  
  // Right wall
  triangles.push_back( Triangle( F, B, D, yellow ) );
  triangles.push_back( Triangle( H, F, D, yellow ) );
  
  Objects->push_back(new Cube(triangles, Diffuse(yellow)));
  triangles.clear();
  
  // Ceiling
  triangles.push_back( Triangle( E, F, G, cyan ) );
  triangles.push_back( Triangle( F, H, G, cyan ) );
  
  Objects->push_back(new Cube(triangles, Diffuse(cyan)));
  triangles.clear();
  
  // Back wall
  triangles.push_back( Triangle( G, D, C, white ) );
  triangles.push_back( Triangle( G, H, D, white ) );
  
  Objects->push_back(new Cube(triangles, Diffuse(white)));
  triangles.clear();
  
  // ---------------------------------------------------------------------------
  // Short block
  
  A = vec3(290,0,114);
  B = vec3(130,0, 65);
  C = vec3(240,0,272);
  D = vec3( 82,0,225);
  
  E = vec3(290,165,114);
  F = vec3(130,165, 65);
  G = vec3(240,165,272);
  H = vec3( 82,165,225);
  
  
  
  // Front
  triangles.push_back( Triangle(E,B,A,red) );
  triangles.push_back( Triangle(E,F,B,red) );
  
  // Front
  triangles.push_back( Triangle(F,D,B,red) );
  triangles.push_back( Triangle(F,H,D,red) );
  
  // BACK
  triangles.push_back( Triangle(H,C,D,red) );
  triangles.push_back( Triangle(H,G,C,red) );
  
  // LEFT
  triangles.push_back( Triangle(G,E,C,red) );
  triangles.push_back( Triangle(E,A,C,red) );
  
  // TOP
  triangles.push_back( Triangle(G,F,E,red) );
  triangles.push_back( Triangle(G,H,F,red) );
  
  Objects->push_back(new Cube(triangles, Diffuse(red)));
  triangles.clear();
  
  // ---------------------------------------------------------------------------
  // Tall block
  
  A = vec3(423,0,247);
  B = vec3(265,0,296);
  C = vec3(472,0,406);
  D = vec3(314,0,456);
  
  E = vec3(423,330,247);
  F = vec3(265,330,296);
  G = vec3(472,330,406);
  H = vec3(314,330,456);
  
  // Front
  triangles.push_back( Triangle(E,B,A,blue) );
  triangles.push_back( Triangle(E,F,B,blue) );
  
  // Front
  triangles.push_back( Triangle(F,D,B,blue) );
  triangles.push_back( Triangle(F,H,D,blue) );
  
  // BACK
  triangles.push_back( Triangle(H,C,D,blue) );
  triangles.push_back( Triangle(H,G,C,blue) );
  
  // LEFT
  triangles.push_back( Triangle(G,E,C,blue) );
  triangles.push_back( Triangle(E,A,C,blue) );
  
  // TOP
  triangles.push_back( Triangle(G,F,E,blue) );
  triangles.push_back( Triangle(G,H,F,blue) );
  
  Objects->push_back(new Cube(triangles, Phong(blue)));
  triangles.clear();
  
  vec3 center = vec3(-0.5,0.7,-0.5);
//  vec3 center = vec3(-0.1,0.2,-1.3);

  float radius = 0.15f;
  Material material = Mirror(red);
  
  Objects->push_back(new Cube(center, radius, material));
  
  center = vec3(-0.0,0.1,-0.5);
  radius = 0.2f;
  material = Glass(trans);
  
//  Objects->push_back(new Cube(center, radius, material));
  

  // ----------------------------------------------
  // Scale to the volume [-1,1]^3
  
  for (vector<Cube*>::iterator itr = Objects->begin(); itr < Objects->end(); itr++) {
    if ((*itr)->getId() == 0){
      //Is cube
      Cube* obj = static_cast<Cube*>(*itr);
      vector<Triangle> t = obj->triangles;
      for (int j = 0; j < t.size(); ++j) {
        obj->triangles[j].v0 *= 2/L;
        obj->triangles[j].v1 *= 2/L;
        obj->triangles[j].v2 *= 2/L;
        
        obj->triangles[j].v0 -= vec3(1,1,1);
        obj->triangles[j].v1 -= vec3(1,1,1);
        obj->triangles[j].v2 -= vec3(1,1,1);
        
        obj->triangles[j].v0.x *= -1;
        obj->triangles[j].v1.x *= -1;
        obj->triangles[j].v2.x *= -1;
        
        obj->triangles[j].v0.y *= -1;
        obj->triangles[j].v1.y *= -1;
        obj->triangles[j].v2.y *= -1;
        
        obj->triangles[j].ComputeNormal();
      }
      obj->Update_Bounds();
    }
  }
}



#endif
