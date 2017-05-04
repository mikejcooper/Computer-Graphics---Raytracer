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



struct Vertex
{
  glm::vec3 position;
};

glm::vec3 findMaxCoordinates(vector<Vertex> vertices);
glm::vec3 findMinCoordinates(vector<Vertex> vertices);


int getVertexIndex(std::string vertex){
  std::stringstream vert(vertex);
  std::string segment;
  std::vector<std::string> seglist;
  while(std::getline(vert, segment, '/'))
  {
	   seglist.push_back(segment);
  }
  return atoi(seglist[0].c_str());
}

void LoadGenericmodel(std::vector<Object*> *Objects){
  vec3 yellow( 0.75f, 0.75f, 0.15f );

  using glm::vec3;
  vector<Vertex> vertices;
  std::vector<Triangle> triangles;
  int vertexCount = 0;
  int minId = 0;
  
  int objectCounter = 0;
  
  vec3 green(  0.15f, 0.75f, 0.15f );
  vec3 red(    0.75f, 0.15f, 0.15f );
  
  std::ifstream myfile;
  myfile.open ("/Users/mikecooper/Desktop/tmp.obj");
  std::string line;
  std::string objectNameCurrent;
  std::string objectNameNext;
  
  while (std::getline(myfile, line))
  {
    std::string a;
    std::istringstream iss(line);
    
    (iss >> a);
    
    if(!a.compare("v")){
	    	float x,y,z;
	    	iss >> x >> y >> z;
	    	Vertex vertex;
	    	vertex.position = vec3(x,y,z);
	    	vertices.push_back(vertex);
	    	vertexCount++;
    }
    if(!a.compare("f")){
	    	cout << "triangle made\n";
	    	std::string v0,v1,v2;
	    	int vertexV0 = 0,vertexV1 = 0,vertexV2 = 0;
	    	iss >> v0 >> v1 >> v2;
	    	vertexV0 = getVertexIndex(v0) - 1;
	    	vertexV1 = getVertexIndex(v1) - 1;
	    	vertexV2 = getVertexIndex(v2) - 1;
      if(objectCounter == 1){
        Triangle triangle = Triangle(vertices[vertexV0].position,vertices[vertexV1].position,vertices[vertexV2].position,green);
        triangles.push_back(triangle);

      }
      if(objectCounter == 2){
        Triangle triangle = Triangle(vertices[vertexV0].position,vertices[vertexV1].position,vertices[vertexV2].position,red);
        triangles.push_back(triangle);

      }
      else {
        Triangle triangle = Triangle(vertices[vertexV0].position,vertices[vertexV1].position,vertices[vertexV2].position,red);
        triangles.push_back(triangle);
      }
      
    }
    if(!a.compare("g")){
        // Assign current to last round
        objectNameCurrent = objectNameNext;
        iss >> objectNameNext;
            
    		// vertexCount = 0;
	    	if(objectCounter > 0){
          if(objectNameCurrent[1] == 'C'){
            Objects->push_back( new Cube(triangles, Diffuse(yellow)) );
            cout << "Cube Objected Added\n";
          }
          else if(objectNameCurrent[1] == 'S'){
            Objects->push_back( new Cube(triangles, Diffuse(yellow)) );
            cout << "Sphere Objected Added\n";
          }

          triangles.clear();
        }
	    	// tempObject.vertices = vertices;
    		// tempObject.vIdMax = vertices.size();
	    	objectCounter++;
    }
  }
  
  //collect last object;
  if(objectNameCurrent[1] == 'C'){
    Objects->push_back( new Cube(triangles, Diffuse(yellow)) );
    cout << "Cube Objected Added\n";
  }
  else if(objectNameCurrent[1] == 'S'){
    Objects->push_back( new Cube(triangles, Diffuse(yellow)) );
    cout << "Sphere Objected Added\n";
  }
  // tempObject.vertices.clear();
  triangles.clear();
  minId = vertexCount-1;
  
  
}

void StoreAsObject(std::vector<Triangle>& triangles, std::vector<Object>& Objects, Material material);

void LoadTestModel( std::vector<Object*> *Objects )
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
  
  // Objects->push_back(new Sphere(center, radius, material));
  
  center = vec3(-0.0,0.1,-0.5);
  radius = 0.2f;
  material = Glass(trans);
  
//  Objects->push_back(new Sphere(center, radius, material));
  
  
  
//  
//  vec3 center = vec3(-0.7,0.8,-0.9);
//  float radius = 0.05f;
//  Material material = Glass(blue);
//  
//  Objects->push_back(new Sphere(center, radius, material));
//
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.8,-0.9);
//    float radius = 0.05f;
//    Material material = Glass(blue);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.6,-0.9);
//    float radius = 0.05f;
//    Material material = Glass(purple);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.4,-0.9);
//    float radius = 0.05f;
//    Material material = Glass(green);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.2,-0.9);
//    float radius = 0.05f;
//    Material material = Glass(red);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.0,-0.9);
//    float radius = 0.05f;
//    Material material = Glass(cyan);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  
////  -------
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.8,-0.7);
//    float radius = 0.05f;
//    Material material = Glass(blue);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.6,-0.7);
//    float radius = 0.05f;
//    Material material = Glass(purple);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.4,-0.7);
//    float radius = 0.05f;
//    Material material = Glass(green);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.2,-0.7);
//    float radius = 0.05f;
//    Material material = Glass(red);
//    Objects->push_back(new Sphere(center, radius, material));
//  }
//  
//  for(int i = 0; i < 8; i++){
//    vec3 center = vec3(-0.7 + 0.2*i,0.0,-0.7);
//    float radius = 0.05f;
//    Material material = Glass(cyan);
//    Objects->push_back(new Sphere(center, radius, material));
//  }

  
  
  
  
  
  // ----------------------------------------------
  // Scale to the volume [-1,1]^3
  
  for (vector<Object*>::iterator itr = Objects->begin(); itr < Objects->end(); itr++) {
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
