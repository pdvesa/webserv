//
// Created by jules on 14/10/2024.
//

#ifndef SPACESCLEAN_HPP
#define SPACESCLEAN_HPP

#include <string>

class SpacesClean {
	private:
		SpacesClean();
		SpacesClean(const SpacesClean &);

		SpacesClean &operator=(const SpacesClean &);

	public:
		~SpacesClean();

		static std::string	cleanLeadingSpaces(const std::string& input);
		static std::string	cleanTrailingSpaces(const std::string& input);
		static std::string	cleanSpaces(const std::string& input);
};



#endif //SPACESCLEAN_HPP
