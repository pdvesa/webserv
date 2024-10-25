//
// Created by jules on 23/10/2024.
//

#include <WebservController.hpp>

int main(int n, char* args[])
{
	if (n != 2 && !args)
		return (EXIT_FAILURE);
//	WebservController	controller(args[1]);
	WebservController	controller;
	controller.run();
	return (EXIT_SUCCESS);
}
