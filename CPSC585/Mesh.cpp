#include "Mesh.h"
#include "glm/glm.hpp"

Mesh::Mesh()
{
	VAO_ID = VBO_POS_ID = VBO_UV_ID = EBO_ID = 0;

	numVertices = numPrimitives = 0;

	primitiveType = GL_TRIANGLES;
}

Mesh::~Mesh()
{
	clear();
}

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

void Mesh::createPlane(float base, float size, float uvScale)
{
	numPrimitives = 2;

	verts = {
		-size,base,-size,
		 size,base,-size,
		 size,base, size,
		-size,base, size,
	};

	UVs = {
		0.0f * uvScale, 0.0f * uvScale,
		1.0f * uvScale, 0.0f * uvScale,
		1.0f * uvScale, 1.0f * uvScale,
		0.0f * uvScale, 1.0f * uvScale,
	};

	indices = {
		0, 1, 2,
		2, 3, 0
	};

	createGLObjects();
}

void Mesh::createBox(float w, float h, float l)
{
	numPrimitives = 6 * 2;

	w *= 0.5f;
	h *= 0.5f;
	l *= 0.5f;

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

	createGLObjects();
}

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

void Mesh::createGLObjects()
{
	// create vertex buffer objects for pos, uv
	glGenBuffers(1, &VBO_POS_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), verts.data(), GL_STATIC_DRAW);
	CHECK_GL;

	glGenBuffers(1, &VBO_UV_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_UV_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * UVs.size(), UVs.data(), GL_STATIC_DRAW);
	CHECK_GL;

	// create index buffer object
	glGenBuffers(1, &EBO_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	CHECK_GL;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECK_GL;

	// create vertex array
	glGenVertexArrays(1, &VAO_ID);
	glBindVertexArray(VAO_ID);
	CHECK_GL;

	GLuint posLayoutLoc = 0;
	GLuint uvLayoutLoc = 1;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_ID);
	glVertexAttribPointer(posLayoutLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_UV_ID);
	glVertexAttribPointer(uvLayoutLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	CHECK_GL;

	glEnableVertexAttribArray(posLayoutLoc);
	glEnableVertexAttribArray(uvLayoutLoc);
	CHECK_GL;

	glBindVertexArray(0);

	glDisableVertexAttribArray(posLayoutLoc);
	glDisableVertexAttribArray(uvLayoutLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECK_GL;
}

void Mesh::render()
{
	glBindVertexArray(VAO_ID);
	CHECK_GL;

	glDrawElements(primitiveType, 3 * numPrimitives, GL_UNSIGNED_INT, (void*)0);
	CHECK_GL;

	glBindVertexArray(0);
	CHECK_GL;
}