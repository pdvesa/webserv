//
// Created by jules on 26/12/2024.
//

#include <RequestBody.hpp>

RequestBody::RequestBody() :
	parsedLength(0)
{ }

bool RequestBody::addData(const std::vector<u_char>& data)
{
	this->parsedLength += data.size();
	this->rawBody.reserve(this->rawBody.size() + data.size());
	this->rawBody.insert(this->rawBody.end(), data.begin(), data.end());
	return (true);
}

size_t RequestBody::getCumulatedSize() const
{
	return (this->parsedLength);
}

const std::vector<u_char>& RequestBody::getContent() const
{
	return (this->rawBody);
}

void RequestBody::clearContent()
{
	this->rawBody.clear();
}
