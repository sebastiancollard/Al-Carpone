#include "GraphicsSystem.h"
#include "constants.h"



using namespace std;

static enum DEBUGMODE {
	FALSE,
	NOJAIL,
	NOALERT,
	NOCOPS
};

extern DEBUGMODE debugmode;


GraphicsSystem::GraphicsSystem() {

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Al Carpone", NULL, NULL);
	if(debugmode != DEBUGMODE::FALSE) window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Al Carpone", NULL, NULL);

	monitor = glfwGetPrimaryMonitor();

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
	shader3D = new Shader("shader3D.vs", "shader3D.fs");
	shader2D = new Shader("shader2D.vs", "shader2D.fs");
	shaderText = new Shader("shaderText.vs", "shaderText.fs");
	shaderDetection = new Shader("shader3D.vs", "shaderDetection.fs");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), 0.0f, static_cast<float>(SCREEN_HEIGHT));
	shaderText->use();
	shaderText->setMat4("projection", projection);

	skybox = new Skybox();
}


////////////////////////////////////////////////////////////////////////////////////////////
// BUFFERS
////////////////////////////////////////////////////////////////////////////////////////////



void GraphicsSystem::clearBuffer() {
	// Specify the color of the background
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Tell OpenGL which Shader Program we want to use
}


void GraphicsSystem::enableDepthBuffer() {
	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);
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

////////////////////////////////////////////////////////////////////////////////////////////
// OTHER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


void GraphicsSystem::toggleFullscreen() {
	if (glfwGetWindowMonitor(window) == nullptr){
		glfwSetWindowMonitor(window, monitor, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	}
	else {
		glfwSetWindowMonitor(window, nullptr, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	}
}
