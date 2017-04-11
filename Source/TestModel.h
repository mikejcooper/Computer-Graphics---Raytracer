#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box
using namespace std;
using namespace glm;
#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>



struct Vertex
{
	glm::vec3 position;
};

glm::vec3 findMaxCoordinates(vector<Vertex> vertices);
glm::vec3 findMinCoordinates(vector<Vertex> vertices);

// Used to describe a triangular surface:
class Triangle
{
public:

	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = v1-v0;
		glm::vec3 e2 = v2-v0;
		normal = glm::normalize( glm::cross( e2, e1 ) );
	}
};

struct Object
{
	std::vector<Triangle> triangles;

	void addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
	{
		Triangle triangle = Triangle(v0,v1,v2,color);
		triangles.push_back(triangle);
	}
};

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

// void printTriangles(std::vector<Triangle>& triangles){
// 	cout << triangles.size() << "\n";
// 	for (int i = 0; i < triangles.size(); ++i)
// 	{
// 		cout << i << ": \n";
// 		cout << triangles[i].vId0 << " " << triangles[i].vId1 << " "  << triangles[i].vId2 << "\n";
// 		cout << triangles[i].normal.x << " " << triangles[i].normal.y << " "  << triangles[i].normal.z << "\n";
// 		cout << triangles[i].color.x << " " << triangles[i].color.y << " "  << triangles[i].color.z << "\n";
// 	}
// }

// void printObjects(std::vector<Object>& objects){
// 	for (int i = 0; i < objects.size(); ++i)
// 	{
// 		cout << objects[i].vIdMax << "\n";
// 		printTriangles(objects[i].triangles);
// 	}
// }

// int findMin(int a,int b, int c,int d){
// 	int array[4] = {a,b,c,d};
// 	int min = INT_MAX;
// 	for (int i = 0; i < 4; ++i)
// 	 {
// 	 	if(array[i] < min){
// 	 		min = array[i];
// 	 	}
// 	 } 
// 	return min;
// }

// int findMax(int a,int b, int c,int d){
// 	int array[4] = {a,b,c,d};
// 	int max = 0;
// 	for (int i = 0; i < 4; ++i)
// 	 {
// 	 	if(array[i] > max){
// 	 		max = array[i];
// 	 	}
// 	 } 
// 	return max;
// }

// glm::vec3 findMaxCoordinates(std::vector<Vertex> vertices){
// 	glm::vec3 max = glm::vec3(0.0f,0.0f,0.0f);
// 	for (int i = 0; i < vertices.size(); ++i)
// 	{
// 		if(vertices[i].position.x > max.x) max.x = vertices[i].position.x;
// 		if(vertices[i].position.y > max.y) max.y = vertices[i].position.y;
// 		if(vertices[i].position.z > max.z) max.z = vertices[i].position.z;
// 	}
// 	return max;
// }

// glm::vec3 findMinCoordinates(std::vector<Vertex> vertices){
// 	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
// 	for (int i = 0; i < vertices.size(); ++i)
// 	{
// 		if(vertices[i].position.x < min.x) min.x = vertices[i].position.x;
// 		if(vertices[i].position.y < min.y) min.y = vertices[i].position.y;
// 		if(vertices[i].position.z < min.z) min.z = vertices[i].position.z;
// 	}
// 	return min;
// }

void LoadGenericmodel(std::vector<Object>& objects){
	using glm::vec3;
	vector<Vertex> vertices;
	Object tempObject;
	// triangles.clear();
	// triangles.reserve( 130 );
	int vertexCount = 0;
	// int maxId = 0;
	int minId = 0;

	int objectCounter = 0;

	vec3 green(  0.15f, 0.75f, 0.15f );

	std::ifstream myfile;
	myfile.open ("mesh/mesh.obj");
	std::string line;

	while (std::getline(myfile, line))
	{
		std::string a;
		std::istringstream iss(line);

		(iss >> a);

		// cout << a << "\n";

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
	    	// cout << v0 << " " << v1 << " " << v2 << ":   ";
	    	vertexV0 = getVertexIndex(v0) - 1;
	    	vertexV1 = getVertexIndex(v1) - 1;
	    	vertexV2 = getVertexIndex(v2) - 1;
	    	// cout << vertexV0 << ", " << vertexV1 << ", " << vertexV2 << "\n";
	    	// maxId = findMax(maxId,vertexV0,vertexV1,vertexV2);
	    	tempObject.addTriangle(vertices[vertexV0].position,vertices[vertexV1].position,vertices[vertexV2].position,green);
	    }
	    if(!a.compare("g")){
    		
    		// vertexCount = 0;
	    	if(objectCounter > 0){
	    		// minId += vertices.size();
	    		// tempObject.CalcBoundingBox();
	    		objects.push_back(tempObject);
	    		// tempObject.vertices.clear();
	    		tempObject.triangles.clear();
	    	}
	    	// tempObject.vertices = vertices;
    		// tempObject.vIdMax = vertices.size();
	    	objectCounter++;
	    }
	}

	//collect last object;
	objects.push_back(tempObject);
	// tempObject.vertices.clear();
	tempObject.triangles.clear();
	minId = vertexCount-1;

	cout << "objects#: " << objects.size() << "\n";

}


#endif