#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW\glfw3.h>

#include <string>
#include <iostream>
#include <vector>

namespace lesson_3n1
{
	class CLoadTexture
	{
	public:

		static void StbiSetFlipVerticallyOnLoad(bool flag);

		static unsigned int LoadTexture(const char* path);

		static unsigned int LoadNormalTexture(const char* path);

		static unsigned int LoadGammaTexture(const char* path);

		static unsigned int LoadHDRTexture(const char* path);

		static unsigned int LoadTexture(const char* path, int wrap_s_par, int wrap_t_par, int min_filter_par, int max_filter_par);

		static unsigned int TextureFromFile(const char* path, std::string directory);

		static unsigned int GammaTextureFromFile(const char* path, std::string directory);

		static unsigned int GetTexture(unsigned int width = 1024, unsigned int height = 1024, GLenum internalFormat = GL_RGB, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE, int wrap_s_par = GL_CLAMP_TO_EDGE, int wrap_t_par = GL_CLAMP_TO_EDGE, int min_filter_par = GL_LINEAR, int max_filter_par = GL_LINEAR);

		static unsigned int GetTexture3D(unsigned int width = 1024, unsigned int height = 1024, unsigned int depth = 1, GLenum internalFormat = GL_RGB, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE, int wrap_s_par = GL_CLAMP_TO_EDGE, int wrap_t_par = GL_CLAMP_TO_EDGE, int min_filter_par = GL_LINEAR, int max_filter_par = GL_LINEAR);

		static unsigned int GetFBOTexture(unsigned int width, unsigned int height, GLenum internalFormat = GL_RGB, GLenum format = GL_RGB);

		static unsigned int GetFloatingPointFBOTexture(unsigned int width, unsigned int height);

		static unsigned int GetDepthMap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetShadowMap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetDepthCubemap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetEnvironmentCubemap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetEnvironmentMipmapCubemap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetFBODepthAttachmentTexture(unsigned int width, unsigned int height);

		static unsigned int LoadCubemap(const std::vector<std::string>& faces);

	};

}
