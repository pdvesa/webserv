//
// Created by jules on 24/12/2024.
//

#ifndef INVALIDCONFIGFILEEXCEPTION_HPP
# define INVALIDCONFIGFILEEXCEPTION_HPP

class InvalidConfigFileException final : public std::runtime_error {
	public:
		InvalidConfigFileException() : std::runtime_error("Invalid config file") {}
		explicit InvalidConfigFileException(const std::string &msg) : std::runtime_error(msg) {}
};

#endif
