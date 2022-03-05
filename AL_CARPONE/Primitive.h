#pragma once

#include <corecrt_math_defines.h>

// Include GLAD
#include<glad/glad.h>

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>


// For rendering any primitives without textures or complicated lighting
class Primitive {

public:

	//Constructor
	Primitive();
	//Destructor
	~Primitive();


	// Create renderable box
	void createBox(float w, float h, float l);
	// Create renderable plane
	void createPlane(float base, float size, float uvScale = 1.0f);
	// Create renderable sphere
	void createSphere(float rad, uint32_t hSegs, uint32_t vSegs);

	// General render function for meshes
	void render();
	// Clear mesh data
	void clear();

protected:
	// General setup needed to generate, bind, and populate buffers as well as 
	// setting the size and location of each layout.
	void createGLObjects();

	// GL_TRIANGLES by default
	GLenum primitiveType;
	// Number of vertices
	unsigned int numVertices;
	// Number of triangles
	unsigned int numPrimitives;

	// Various buffer IDs for relevant buffer functions
	GLuint VAO_ID;
	GLuint EBO_ID;
	GLuint VBO_POS_ID;
	GLuint VBO_UV_ID;

	// Vectors to hold relevant information about the mesh
	std::vector<GLuint> indices;
	std::vector<GLfloat> verts;
	std::vector<GLfloat> UVs;


};
