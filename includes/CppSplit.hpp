//
// Created by jules on 09/10/2024.
//

#ifndef CPPSPLIT_HPP
#define CPPSPLIT_HPP

#include <string>
#include <sstream>
#include <vector>

class CppSplit {
	private:
		CppSplit();
		CppSplit(const CppSplit &);

		CppSplit &operator=(const CppSplit &);

	public:
		~CppSplit();
		static std::vector<std::string> cppSplit(const std::string &string, char delim);
};



#endif //CPPSPLIT_HPP
