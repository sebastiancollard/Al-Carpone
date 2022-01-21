#include "Mesh.h"
#include "glm/glm.hpp"

// Mesh constructor
Mesh::Mesh()
{
	// Initializes buffer IDs
	VAO_ID = VBO_POS_ID = VBO_UV_ID = EBO_ID = 0;
	// Initialized number of verts and primitives
	numVertices = numPrimitives = 0;
	// We'll be working with triangles for OpenGL
	primitiveType = GL_TRIANGLES;
}

// Mesh deconstructor
Mesh::~Mesh()
{
	clear();
}

// Clears buffers and resets IDs
void Mesh::clear()
{
	if (VBO_POS_ID)
	{
		glDeleteBuffers(1, &VBO_POS_ID);
		VBO_POS_ID = 0;
	}

	if (VBO_UV_ID)
	{
		glDeleteBuffers(1, &VBO_UV_ID);
		VBO_UV_ID = 0;
	}

	if (EBO_ID)
	{
		glDeleteBuffers(1, &EBO_ID);
		EBO_ID = 0;
	}

	if (VAO_ID)
	{
		glDeleteVertexArrays(1, &VAO_ID);
		VAO_ID = 0;
	}
}

// Create renderable box
void Mesh::createPlane(float base, float size, float uvScale)
{
	// A plane consists of 2 triangles
	numPrimitives = 2;

	// Use inputs to define vertices
	verts = {
		-size,base,-size,
		 size,base,-size,
		 size,base, size,
		-size,base, size,
	};

	// Define UV coords for each vertex
	UVs = {
		0.0f * uvScale, 0.0f * uvScale,
		1.0f * uvScale, 0.0f * uvScale,
		1.0f * uvScale, 1.0f * uvScale,
		0.0f * uvScale, 1.0f * uvScale,
	};

	// Defind order in which vertices are drawing on the screen
	indices = {
		0, 1, 2,
		2, 3, 0
	};

	// General setup needed to generate, bind, and populate buffers as well as 
	// setting the size and location of each layout.
	createGLObjects();
}

// Create renderable plane
void Mesh::createBox(float w, float h, float l)
{
	// A box consists of 6 faces, each are 2 triangles
	numPrimitives = 6 * 2;

	// Update parameters to halfextents
	w *= 0.5f;
	h *= 0.5f;
	l *= 0.5f;

	// Use halfextents to define vertices/triangles
	verts = {
		// bottom face
		w, -h, l,
		-w, -h, l,
		-w, -h, -l,
		w, -h, -l,

		// top face
		-w, h, l,
		w, h, l,
		w, h, -l,
		-w, h, -l,

		// left face
		-w, -h, -l,
		- w, -h,  l,
		- w,  h,  l,
		- w,  h, -l,

		// right face
		w, -h,  l,
		w, -h, -l,
		w,  h, -l,
		w,  h,  l,
		
		// front face
		-w, -h,  l,
		w, -h,  l,
		w,  h,  l,
		- w,  h,  l,

		// back face
		w, -h, -l,
		-w, -h, -l,
		-w,  h, -l,
		w,  h, -l
	};

	// Define UV coords for each vertex
	UVs = {
		// bottom face
		0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		0.0f, 1.0f,

		// top face
		 0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		 0.0f, 1.0f,

		 // left face
		 0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		 0.0f, 1.0f,

		 // right face
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		// front face
		 0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		 0.0f, 1.0f,

		 // back face
		0.0f, 0.0f,
		 1.0f, 0.0f,
		 1.0f, 1.0f,
		0.0f, 1.0f
	};

	// Define order in which vertices are drawing on the screen
	indices = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};

	// General setup needed to generate, bind, and populate buffers as well as 
	// setting the size and location of each layout.
	createGLObjects();
}

// Create renderable sphere
void Mesh::createSphere(float rad, uint32_t hSegs, uint32_t vSegs)
{
	numPrimitives = hSegs * vSegs * 2;

	float dphi = (float)(2.0 * M_PI) / (float)(hSegs);
	float dtheta = (float)(M_PI) / (float)(vSegs);

	for (uint32_t v = 0; v <= vSegs; ++v)
	{
		float theta = v * dtheta;

		for (uint32_t h = 0; h <= hSegs; ++h)
		{
			float phi = h * dphi;

			float x = std::sin(theta) * std::cos(phi);
			float y = std::cos(theta);
			float z = std::sin(theta) * std::sin(phi);

			verts.insert(verts.end(), { rad * x, rad * y, rad * z });
			UVs.insert(UVs.end(), { 1.0f - (float)h / hSegs, (float)v / vSegs });
		}
	}

	for (uint32_t v = 0; v < vSegs; v++)
	{
		for (uint32_t h = 0; h < hSegs; h++)
		{
			uint32_t topRight = v * (hSegs + 1) + h;
			uint32_t topLeft = v * (hSegs + 1) + h + 1;
			uint32_t lowerRight = (v + 1) * (hSegs + 1) + h;
			uint32_t lowerLeft = (v + 1) * (hSegs + 1) + h + 1;

			std::vector< unsigned int > tri0;
			std::vector< unsigned int > tri1;

			tri0.push_back(lowerLeft);
			tri0.push_back(lowerRight);
			tri0.push_back(topRight);

			tri1.push_back(lowerLeft);
			tri1.push_back(topRight);
			tri1.push_back(topLeft);

			indices.insert(indices.end(), tri0.begin(), tri0.end());
			indices.insert(indices.end(), tri1.begin(), tri1.end());
		}
	}

	createGLObjects();
}

// General setup needed to generate, bind, and populate buffers as well as 
// setting the size and location of each layout.
void Mesh::createGLObjects()
{
	// Create vertex buffer objects (VBOs) for pos and uv

	// Generate buffer ID for the position VBO
	glGenBuffers(1, &VBO_POS_ID);
	// Binds the position VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_ID);
	// Populates the array buffer with the vertex data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), verts.data(), GL_STATIC_DRAW);
	// Error check
	CHECK_GL;

	// Generate buffer ID for the uv VBO
	glGenBuffers(1, &VBO_UV_ID);
	// Binds the uv VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_UV_ID);
	// Populates the array buffer with the UV data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * UVs.size(), UVs.data(), GL_STATIC_DRAW);
	// Error check
	CHECK_GL;

	// Create index buffer object (EBO)


	// Generate buffer ID for the EBO
	glGenBuffers(1, &EBO_ID);
	// Binds the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);
	// Populates the array buffer with the indice data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	// Error check
	CHECK_GL;

	// Unbind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Error check
	CHECK_GL;

	// Create vertex array object (VAO)

	// Generate buffer ID for the VAO
	glGenVertexArrays(1, &VAO_ID);
	// Binds the VAO
	glBindVertexArray(VAO_ID);
	// Error check
	CHECK_GL;

	// Layout locations used for the vertex shader
	GLuint posLayoutLoc = 0;
	GLuint uvLayoutLoc = 1;

	// Bind the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);

	// Bind the VBOs, specify their layout location, size, type, stride, etc...
	glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_ID);
	glVertexAttribPointer(posLayoutLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_UV_ID);
	glVertexAttribPointer(uvLayoutLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// Error check
	CHECK_GL;

	// Enable the position vertex attribute 
	glEnableVertexAttribArray(posLayoutLoc);
	// Enable the UV vertex attribute 
	glEnableVertexAttribArray(uvLayoutLoc);
	// Error check
	CHECK_GL;

	// Unbind VAO
	glBindVertexArray(0);

	// Disable the position vertex attribute 
	glDisableVertexAttribArray(posLayoutLoc);
	// Disable the UV vertex attribute 
	glDisableVertexAttribArray(uvLayoutLoc);

	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Error check
	CHECK_GL;
}

void Mesh::render()
{
	// Bind the VAO
	glBindVertexArray(VAO_ID);
	// Error check
	CHECK_GL;

	// Render
	glDrawElements(primitiveType, 3 * numPrimitives, GL_UNSIGNED_INT, (void*)0);
	// Error check
	CHECK_GL;

	// Unbind VAO
	glBindVertexArray(0);
	// Error check
	CHECK_GL;
}