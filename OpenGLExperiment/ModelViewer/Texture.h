#pragma once
#include <cstdint>
#include <memory>
#include <iostream>
#include <algorithm>
#include <gl\glew.h>
#include <exception>
#include <fstream>
namespace GL
{
	class ArrayBuffer
	{
	public:
		operator GLuint() const
		{
			return Handler;
		}
		GLuint Handler;
	};

	class Texture2D
	{
	public:
		Texture2D(const std::string& fileName)
		{
			auto ext = fileName.substr(fileName.find_last_of('.')+1);
			if (ext == "ppm")
			{
				std::ifstream file(fileName, std::ios::binary);
				if (!file.is_open()) return;
				LoadFromPPM(file);
				return;
			}
			throw std::runtime_error("Unsupported texture format : " + ext);
		}

		//static std::shared_ptr<Texture2D> LoadFromFile(const std::string& fileName)
		//{
		//	auto ext = fileName.substr(fileName.find_last_of('.') + 1);
		//	if (ext == "ppm")
		//	{
		//		ifstream file(fileName, ios::binary);
		//		if (!file.is_open()) return;
		//		LoadFromPPM(file);
		//		return;
		//	}
		//	throw std::runtime_error("Unsupported texture format : " + ext);
		//}

		Texture2D(size_t width,size_t height, uint8_t* data, GLenum pixelFormat = GL_RGB)
		{
			LoadFromMemery(width, height, data, pixelFormat);
		}

		operator GLuint() const
		{
			return Handler;
		}

		void LoadFromMemery(size_t width, size_t height, uint8_t* data, GLenum pixelFormat = GL_RGB)
		{
			Width = width;
			Height = height;
			glGenTextures(1, &Handler);
			glBindTexture(GL_TEXTURE_2D, Handler);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		void LoadFromPPM(std::istream& fin)
		{
			char p6[10];
			size_t width, height, depth;

			// load file header for image info 
			fin >> p6 >> width >> height >> depth;
			char ch;
			fin.get(ch);
			while (ch != '\n')
				fin.get(ch);
			//sscanf_s(cline, "%s %s %s %s", p6, width, height, depth);

			auto data = std::unique_ptr<uint8_t []>(new uint8_t[3 * width*height]);
			fin.read((char*) data.get(), sizeof(uint8_t) * 3 * width * height);

			LoadFromMemery(width, height, data.get());
		}

		Texture2D();
		~Texture2D();

		size_t Width;
		size_t Height;
		size_t Depth;
		//uint8_t* Pixels;

		GLuint Handler;
	};
}
