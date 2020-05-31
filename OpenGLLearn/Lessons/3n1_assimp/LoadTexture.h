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

		static unsigned int TextureFromFile(const char* path, std::string directory);

	};

}
