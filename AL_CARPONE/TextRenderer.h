#pragma once
#include <iostream>
#include <map>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"

//Code taken and adjusted from https://learnopengl.com/In-Practice/Text-Rendering

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextRenderer 
{
public:
    std::map<GLchar, Character> Characters;
    
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1,1,1));
    
    int initFont();        //Functionality based off of the contents of "main" in the example. Sets up the font for use.
private:
    unsigned int VAO, VBO;

};