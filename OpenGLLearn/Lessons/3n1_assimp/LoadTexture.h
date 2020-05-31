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

		static unsigned int loadTexture(const char* path, int wrap_s_par, int wrap_t_par, int min_filter_par, int max_filter_par);

		static unsigned int TextureFromFile(const char* path, std::string directory);

	};

}
