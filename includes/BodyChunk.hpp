#pragma once

#include <vector>
#include <string>

class BodyChunk
{
	private:
		int chunkSize;
		std::string chunkContent;
	public:
		BodyChunk(int size, std::string content);
		~BodyChunk();
		int	getSize() const;
		std::string	getContent() const;
};
