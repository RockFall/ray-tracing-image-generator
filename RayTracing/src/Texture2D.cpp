#include "Texture2D.h"

#include <fstream>
#include <iostream>
#include <assert.h>



Texture2D::Texture2D(const std::string& path)
	: path_(path), width_(), height_()
{
	std::ifstream in(path.c_str(), std::ios::binary);
	std::string type;
	long width = 0;
	long height = 0;
	long pixel_max = 0;

	in >> type >> width >> height >> pixel_max;
}

Texture2D::~Texture2D()
{

}