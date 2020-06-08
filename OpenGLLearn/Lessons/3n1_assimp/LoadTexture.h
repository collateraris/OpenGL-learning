#pragma once

#include <string>
#include <iostream>
#include <vector>

namespace lesson_3n1
{
	class CLoadTexture
	{
	public:
		static unsigned int loadTexture(const char* path);

		static unsigned int loadNormalTexture(const char* path);

		static unsigned int loadGammaTexture(const char* path);

		static unsigned int loadTexture(const char* path, int wrap_s_par, int wrap_t_par, int min_filter_par, int max_filter_par);

		static unsigned int TextureFromFile(const char* path, std::string directory);

		static unsigned int GammaTextureFromFile(const char* path, std::string directory);

		static unsigned int GetFBOTexture(unsigned int width, unsigned int height);

		static unsigned int GetDepthMap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetDepthCubemap(unsigned int width = 1024, unsigned int height = 1024);

		static unsigned int GetFBODepthAttachmentTexture(unsigned int width, unsigned int height);

		static unsigned int LoadCubemap(const std::vector<std::string>& faces);
	};

}
