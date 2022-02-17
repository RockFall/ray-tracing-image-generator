#include "Window.h"

#include <iostream>
#include <assert.h>


static bool has_glfw_initialized = false;

static void GLFWErrorCallback(int error, const char* description) {
	std::cout << "GLFW Error (" << error << "): " << description << std::endl;
}

Window::Window(const WindowConfigs& configs)
{
	Init(configs);
}

Window::~Window()
{
	// We may change this due to multiple windows
	Shutdown();
}

void Window::Init(const WindowConfigs& configs)
{
	// Setting up WindowData
	data_.title = configs.title;
	data_.width = configs.width;
	data_.height = configs.height;

	std::cout << "Creating window " << configs.title << "(" << configs.width << ", " << configs.height << ")" << std::endl;

	// Starts GLFW if not yet
	if (!has_glfw_initialized) {
		// Initializes GLFW
		int success = glfwInit();
		// Specify the client API version (in this case OpenGL 4.5) that 
		// the created window must be compatible with
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		// From 'Core Profile' (Mordern functions) or 'Compatibility Profile' (Modern + Outdated)
		// - Using CORE PROFILE
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Makes sure it has initialized successfully
		if (!success) {
			std::cout << "Could not initialize GLFW!" << std::endl;
			assert(success);
		}

		glfwSetErrorCallback(GLFWErrorCallback);

		has_glfw_initialized = true;
	}

	window_ = glfwCreateWindow(configs.width, configs.height, data_.title.c_str(), nullptr, nullptr);
	if (window_ == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		assert(false);
	}
		
	// Tells GLFW that window_ is where the current context must be
	glfwMakeContextCurrent(window_);
	// Load Glad
	int glad_loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (glad_loaded == 0) {
		std::cout << "Failed to initialize Glad!" << std::endl;
		assert(false);
	}

	std::cout << "OpenGL Info:" << std::endl;
	std::cout << "	 Vendor:   {0}" << glGetString(GL_VENDOR) << std::endl;
	std::cout << "	 Renderer: {0}" << glGetString(GL_RENDERER) << std::endl;
	std::cout << "	 Version:  {0}" << glGetString(GL_VERSION) << std::endl;
		
	// Used mainly to connect the callback and event system to GLFW
	glfwSetWindowUserPointer(window_, &data_);
	set_vsync(true);
}

void Window::OnUpdate()
{
	glfwPollEvents();
	SwapBuffers();
}

void Window::set_vsync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);
	data_.vsync = enabled;
}

bool Window::is_vsync() const
{
	return data_.vsync;
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window_);
}

void Window::Shutdown()
{
	glfwDestroyWindow(window_);
}