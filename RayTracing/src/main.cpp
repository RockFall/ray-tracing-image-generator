#include <iostream>

#include <memory>
#include <string>
#include<sstream>
#include <iostream>
#include <fstream>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SceneData.h"
#include "Camera.h"
#include "Texture2D.h"
#include "RayTracing.h"
#include "IOHandler.h"

void InputInterface(std::string& input_file, std::string& output_file, int& WIDTH, int& HEIGHT) {
	std::cout << "Insert input and output file names ( i.e. 'in.txt out.ppm' )" << std::endl;
	std::cout << "> Optionally you also can pass width and height of the output ( i.e. 'in.in out.ppm 800 600' )" << std::endl;
	std::cout << "> (Note that those parameters can also be passed through command line)" << std::endl;
	std::string input;
	std::getline(std::cin, input);
	std::istringstream ss(input);
	int count = 0;
	while (std::getline(ss, input, ' ')) {
		if (count == 0)
			input_file = input;
		if (count == 1)
			output_file = input;
		if (count == 2)
			WIDTH = std::stoi(input);
		if (count == 3)
			HEIGHT = std::stoi(input);
		++count;
	}
}

int main(int argc, char *argv[]) {
	// Base variables
	std::string input_file, output_file;
	int WIDTH = 800, HEIGHT = 600;
	int DEPTH = 4;

	switch (argc)
	{
	case 1: {
		InputInterface(input_file, output_file, WIDTH, HEIGHT);
	}
	break;
	case 3: {
		input_file = argv[1];
		output_file = argv[2];
	}
	break;
	case 5: {
		input_file = argv[1];
		output_file = argv[2];
		WIDTH = std::stoi(argv[3]);
		HEIGHT = std::stoi(argv[4]);
	}
	break;
	default: {
		std::cout << "Invalid parameters, insert input and output files (optionally width and height too)" << std::endl;
		std::cout << ">> tp03.exe input.in output.ppm 800 600" << std::endl;
		return 0;
	}
	break;
	}

	// Scene Input
	SceneData scene = SceneData();
	if (IOHandler::GetSceneData(scene, input_file) == false) {
		return 1;
	}
	std::cout << "\nScene data received successfully!" << std::endl;

	// Set-up Camera
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(
		scene.camera.pos, 
		scene.camera.target, 
		scene.camera.up_vector, 
		scene.camera.fov,
		WIDTH,
		HEIGHT);

	// Create an output ppm file
	IOHandler::WriteFile(scene, camera, output_file, WIDTH, HEIGHT, DEPTH);
	std::cout << "\nImage generated! File: " << output_file << std::endl;

	return 0;
}
