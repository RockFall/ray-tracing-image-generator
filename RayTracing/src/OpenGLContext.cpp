#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <assert.h>

OpenGLContext::OpenGLContext(GLFWwindow* window) : window_(window)
{
	assert(window != NULL, "The window given to OpenGLContext is NULL");
}

void OpenGLContext::Init()
{
	// Tells GLFW that window_ is where the current context must be
	glfwMakeContextCurrent(window_);
	// Load Glad
	int glad_loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	assert(glad_loaded, "Failed to initialize Glad!");


	std::cout << "OpenGL Info:" << std::endl;
	std::cout << "	 Vendor:   {0}" << glGetString(GL_VENDOR) << std::endl;
	std::cout << "	 Renderer: {0}" << glGetString(GL_RENDERER) << std::endl;
	std::cout << "	 Version:  {0}" << glGetString(GL_VERSION) << std::endl;
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(window_);
}
