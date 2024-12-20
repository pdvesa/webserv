//
// Created by jules on 19/12/2024.
//

#ifndef VECBUFFCMP_HPP
#define VECBUFFCMP_HPP

#include <vector>

class VecBuffCmp {
	public:
		static int	vecBuffCmp(std::vector<unsigned char> vec, size_t startVec, const void* buff, size_t startBuff,
		size_t n);

		VecBuffCmp() = delete;
		VecBuffCmp(const VecBuffCmp &) = delete;
		~VecBuffCmp() = delete;

		VecBuffCmp &operator=(const VecBuffCmp &) = delete;
};



#endif //VECBUFFCMP_HPP
