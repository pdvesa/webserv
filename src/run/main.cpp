//
// Created by jules on 23/10/2024.
//

#include <WebservController.hpp>

int main(int n, char* args[])
{
	if (n != 2)
		return (1);

	WebservController	controller(args[1]);
	controller.run();
	return (0);
}
