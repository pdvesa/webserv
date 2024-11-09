#include "../includes/ExtractChunk.hpp"
#include <iostream>


std::string ExtractChunk::remove_chunk(std::vector<unsigned char> &body)
{
	const char *eof = "\r\n";
	// get size_str
	// strtol hexadecimal into a long, using strtol(str, NULL, 16);
	// skip \r\n\r\n after size, then read size amount of bytes as body
	// pass size and body into BodyChunk;
	auto it = std::search(body.begin(), body.end(), eof, eof + strlen(eof));
	it += strlen(eof);
	std::string sizestr(body.begin(), it);
	long reqSize = strtol(sizestr.c_str(), NULL, 16);
	if (!reqSize)
		return "";
	std::cout << "Size of reqsize: " << reqSize << std::endl;
	auto endit = it + reqSize;
	// find new string until next eof str
	std::string targetChunk(it ,endit);
	body.erase(body.begin(), endit);
	return targetChunk;
}

int	main(void)
{
	std::string testreq = "0\r\ni love baboons,they are my favorite animal\r\n3\r\nhey\r\n0\r\n";
	std::vector<unsigned char> baboon;
	for (const auto i : testreq)
		baboon.push_back(i);
	std::cout << ExtractChunk::remove_chunk(baboon) << std::endl;
//	for (const auto i : baboon)
//		std::cout << i;
//	std::cout << std::endl;
}
