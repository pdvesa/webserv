//
// Created by jules on 24/12/2024.
//

#include <HttpRequest.hpp>
#include <iostream>
#include <vector>
#include <ctime>


int	main()
{
	// Example of a raw HTTP POST request
	std::vector<u_char> rawHttpRequest = {
		// Request Line
		'P', 'O', 'S', 'T', ' ', '/', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',

		// Headers
		'H', 'o', 's', 't', ':', ' ', 'w', 'w', 'w', '.', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm', '\r', '\n',
		'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', ' ', 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n', '\r', '\n',
		'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' ', '1', '8', '\r', '\n',  // Length of JSON body (bytes)

		'\r', '\n', // End of headers

		// Body (JSON data as an example)
		'{', '"', 'k', 'e', 'y', '"', ':', '"', 'v', 'a', 'l', 'u', 'e', '"', '}', '\r', '\n'
	};

	// Example testing: Print the raw request to the console
	for (const u_char c : rawHttpRequest) {
		std::cout << c;
	}
	std::cout << std::endl;
	int	i = 0;
	std::srand(std::time(nullptr));
	HttpRequest request(nullptr);
	while (i < rawHttpRequest.size())
	{
		int	random = std::rand() % rawHttpRequest.size();
		if (i + random > rawHttpRequest.size())
			random = rawHttpRequest.size() - i;
		request.parseData(rawHttpRequest.data() + i, random);
		i += random;
		std::cout << "RANDOM PARSING" << std::endl;
	}
	std::cout << request.getRequestState() << std::endl;
	std::cout << request.getMethod() << std::endl;
	std::cout << request.getTarget() << std::endl;
	std::cout << request.getVersion() << std::endl;
	std::cout << request.getHeader("Host") << std::endl;
	std::cout << request.getHeader("Content-Type") << std::endl;
	std::cout << request.getHeader("Content-Length") << std::endl;
	auto body = request.getBody();
	for (const u_char c : body) {
		std::cout << c;
	}
	std::cout << std::endl;
	return (0);
}
