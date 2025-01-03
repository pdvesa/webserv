//
// Created by jules on 19/12/2024.
//

#include <VecBuffCmp.hpp>

int VecBuffCmp::vecBuffCmp(const std::vector<unsigned char>& vec, size_t startVec, const void* buff, size_t startBuff,
	size_t n)
{
	const auto buffPtr = static_cast<const unsigned char*>(buff);

	size_t i = 0;
	while (i < n) {
		unsigned char vecChar = vec[startVec + i];
		unsigned char buffChar = buffPtr[startBuff + i];

		if (vecChar != buffChar) {
			return (vecChar - buffChar);
		}
		i++;
	}

	return (0);
}
