#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <memory>

// Holds title and size data
struct WindowConfigs {
	std::string title;
	int width;
	int height;

	WindowConfigs(const std::string& title = "Window",
		int width = 1280,
		int height = 720)
		: title(title), width(width), height(height) { }
};

class Window
{
public:
	Window(const WindowConfigs& configs);
	virtual ~Window();

	void OnUpdate();

	inline int get_width() const { return data_.width; }
	inline int get_height() const { return data_.height; }

	// Window Attributes
	void set_vsync(bool enabled);
	bool is_vsync() const;

	void SwapBuffers();

	inline virtual void* get_native_window() const { return window_;  };
private:
	virtual void Init(const WindowConfigs& configs);
	virtual void Shutdown();
private:
	GLFWwindow* window_;

	struct WindowData {
		std::string title;
		int width, height;
		bool vsync;
	};

	WindowData data_;
};
