#include "HttpResponse.hpp"


HttpResponse::HttpResponse()
{
};

void	HttpResponse::create_responseline(int code)
{
	std::string message = httpErrors.at(code);
	std::string output = m_status_line + ' ' + std::to_string(code) + ' ' + message;

	m_status_line = message;
	std::cout << output;
}
