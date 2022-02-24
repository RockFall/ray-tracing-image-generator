#include "Texture2D.h"


#include <iostream>
#include <assert.h>



Texture2D::Texture2D(const std::string& path)
	: path_(path)
{
    load_ppm(path);
}

Texture2D::~Texture2D()
{

}

// http://josiahmanson.com/prose/optimize_ppm/
void Texture2D::load_ppm(const std::string& name)
{
    std::ifstream f(name.c_str(), std::ios::binary);
    if (f.fail())
    {
        std::cout << "Could not open file: " << name << std::endl;
        return;
    }

    // get type of file
    eat_comment(f);
    int mode = 0;
    std::string s;
    f >> s;
    if (s == "P3")
        mode = 3;
    else if (s == "P6")
        mode = 6;

    // get w
    eat_comment(f);
    f >> img_.w;

    // get h
    eat_comment(f);
    f >> img_.h;

    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;

    // error checking
    if (mode != 3 && mode != 6)
    {
        std::cout << "Unsupported magic number" << std::endl;
        f.close();
        return;
    }
    if (img_.w < 1)
    {
        std::cout << "Unsupported width: " << img_.w << std::endl;
        f.close();
        return;
    }
    if (img_.h < 1)
    {
        std::cout << "Unsupported height: " << img_.h << std::endl;
        f.close();
        return;
    }
    if (bits < 1 || bits > 255)
    {
        std::cout << "Unsupported number of bits: " << bits << std::endl;
        f.close();
        return;
    }

    // load image data
    img_.data.resize(img_.w * img_.h);

    if (mode == 6)
    {
        f.get();
        f.read((char*)&img_.data[0], img_.data.size() * 3);
    }
    else if (mode == 3)
    {
        for (int i = 0; i < img_.data.size(); i++)
        {
            int v;
            f >> v;
            img_.data[i].r = v;
            f >> v;
            img_.data[i].g = v;
            f >> v;
            img_.data[i].b = v;
        }
    }

    // close file
    f.close();
}

void Texture2D::eat_comment(std::ifstream& f)
{
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}