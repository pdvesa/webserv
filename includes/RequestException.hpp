//
// Created by jules on 23/12/2024.
//

#ifndef REQUESTEXCEPTION_HPP
# define REQUESTEXCEPTION_HPP

#include <stdexcept>

class InvalidRequestException final : public std::runtime_error {
	public:
	InvalidRequestException() : std::runtime_error("Invalid request") {}
	explicit InvalidRequestException(const std::string &msg) : std::runtime_error(msg) {}
};

class NotFoundException final : public std::runtime_error {
	public:
	NotFoundException() : std::runtime_error("No route found") {}
};

class MethodNotAllowedException final : public std::runtime_error {
	public:
	MethodNotAllowedException() : std::runtime_error("Method not allowed") {}
};

class ForbiddenException final : public std::runtime_error {
	public:
	ForbiddenException() : std::runtime_error("Forbidden") {}
};

class NotImplementedException final : public std::runtime_error {
	public:
	NotImplementedException() : std::runtime_error("Not implemented") {}
};

class RequestBodyTooLargeException final : public std::runtime_error {
	public:
	RequestBodyTooLargeException() : std::runtime_error("Request body too large") {}
};

class RequestContentLengthMissingException final : public std::runtime_error {
	public:
	RequestContentLengthMissingException() : std::runtime_error("Request content length missing") {}
};

#endif
