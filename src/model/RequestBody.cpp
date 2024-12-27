//
// Created by jules on 26/12/2024.
//

#include <RequestBody.hpp>

bool RequestBody::addData(const std::vector<u_char>& data)
{
	this->rawBody.reserve(this->rawBody.size() + data.size());
	this->rawBody.insert(this->rawBody.end(), data.begin(), data.end());
	return (true);
}

size_t RequestBody::getSize() const
{
	return (this->rawBody.size());
}

const std::vector<u_char>& RequestBody::getContent() const
{
	return (this->rawBody);
}
