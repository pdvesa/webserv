//
// Created by jules on 09/10/2024.
//

#ifndef STRICTUTOI_HPP
#define STRICTUTOI_HPP

#include <string>

class StrictUtoi {
	private:
		StrictUtoi();
		StrictUtoi(const StrictUtoi&);

		StrictUtoi& operator=(const StrictUtoi&);

  	public:
		~StrictUtoi();

		static u_int	strictUtoi(const std::string & str);
};



#endif //STRICTUTOI_HPP
