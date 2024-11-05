#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>

class ExtractChunk
{
	public:
		~ExtractChunk();
		static std::string remove_chunk(std::vector<unsigned char> &body);
};
