//
// Created by jules on 23/10/2024.
//

#include <WebservController.hpp>

int main(int n, char *args[])
{
	if (n != 2)
	{
		std::cerr << "Please include a valid config file\n";
		return (EXIT_FAILURE);
	}
	try {
	WebservController	controller(args[1]);
	controller.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return (EXIT_SUCCESS);
}
