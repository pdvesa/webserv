#include "BodyChunk.hpp"

BodyChunk::BodyChunk(int size, std::string content): chunkSize(size), chunkContent(content){
}

BodyChunk::~BodyChunk() {	
}

int	BodyChunk::getSize() const
{
	return chunkSize;
}

std::string BodyChunk::getContent() const
{
	return chunkContent;
}
