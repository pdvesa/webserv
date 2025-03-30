//
// Created by jules on 23/12/2024.
//

#ifndef REQUESTEXCEPTION_HPP
# define REQUESTEXCEPTION_HPP

#include <RequestState.hpp>
#include <stdexcept>

class RequestException : public std::runtime_error {
	public:
	explicit RequestException(const std::string& message) : std::runtime_error(message) {}
	virtual int getCode() = 0;
};

class RequestParsingException : public RequestException {
	public:
	explicit RequestParsingException(const std::string& message) : RequestException(message) {}
	virtual t_request_state getState() = 0;
};

class InvalidRequestException : public RequestParsingException {
	public:
	InvalidRequestException() : RequestParsingException("Invalid request") {}
	explicit InvalidRequestException(const std::string &msg) : RequestParsingException(msg) {}
	int getCode() override { return (400); }
	t_request_state getState() override { return (t_request_state::REQUEST_INVALID);}
};

class NotFoundException final : public RequestException {
	public:
	NotFoundException() : RequestException("No route found") {}
	int getCode() override { return (404); }
};

class MethodNotAllowedException final : public RequestException {
	public:
	MethodNotAllowedException() : RequestException("Method not allowed") {}
	int getCode() override { return (405); }
};

class ForbiddenException final : public RequestException {
	public:
	ForbiddenException() : RequestException("Forbidden") {}
	int getCode() override { return (403); }
};

class IamATeapotException final : public RequestParsingException {
	public:
	IamATeapotException() : RequestParsingException("I am a teapot") {}
	explicit IamATeapotException(const std::string &msg) : RequestParsingException(msg) {}
	int getCode() override { return (418); }
	t_request_state getState() override { return (I_AM_A_TEAPOT); }
};

class NotImplementedException final : public RequestParsingException {
	public:
	NotImplementedException() : RequestParsingException("Not implemented") {}
	int getCode() override { return (501); }
	t_request_state getState() override { return (REQUEST_UNIMPLEMENTED); }
};

class RequestBodyTooLargeException final : public RequestParsingException {
	public:
	RequestBodyTooLargeException() : RequestParsingException("Request body too large") {}
	int getCode() override { return (413); }
	t_request_state getState() override { return (REQUEST_BODY_TOO_LARGE); }
};

class RequestContentLengthMissingException final : public RequestParsingException {
	public:
	RequestContentLengthMissingException() : RequestParsingException("Request content length missing") {}
	int getCode() override { return (411); }
	t_request_state getState() override { return (REQUEST_LEN_REQUIRED); }
};

class HttpVersionNotSupportedException final : public RequestParsingException {
	public:
	HttpVersionNotSupportedException() : RequestParsingException("HTTP version not supported") {}
	int getCode() override { return (505); }
	t_request_state getState() override { return (HTTP_VERSION_NOT_SUPPORTED); }
};

#endif
