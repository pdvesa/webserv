//
// Created by jules on 16/10/2024.
//

#ifndef ISBLANK_HPP
# define ISBLANK_HPP

#include <string>

class IsBlank {
	private:
	IsBlank();
	IsBlank(const IsBlank &);

	IsBlank &operator=(const IsBlank &);

	public:
		~IsBlank();

		static bool	isBlank(const std::string&);
};



#endif //ISBLANK_HPP
