#include "GraphicsSystem.h"
#include "constants.h"



using namespace std;


GraphicsSystem::GraphicsSystem() {

	// Initialize GLFW
	glfwInit();
	cout << 0 << endl;

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Al Carpone", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Generates Shader object using shaders default.vert and default.frag
	cout << 1 << endl;
	shader3D = std::shared_ptr<Shader>(new Shader("shader3D.vs", "shader3D.fs"));
	cout << 2 << endl;
	shader2D = std::shared_ptr<Shader>(new Shader("shader2D.vs", "shader2D.fs"));
	cout << 3 << endl;
}


void GraphicsSystem::clearBuffer() {
	// Specify the color of the background
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Tell OpenGL which Shader Program we want to use
}


void GraphicsSystem::swapBuffers() {
	// Swap the back buffer with the front buffer
	glfwSwapBuffers(window);
}


void GraphicsSystem::cleanup() {
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}