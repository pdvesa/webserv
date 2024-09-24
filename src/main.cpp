#include <Controller.hpp>

# ifndef PORT_TEST //placeholder
#  define PORT_TEST 8080 
# endif
# ifndef ADDR_TEST //placeholder
#  define ADDR_TEST "127.0.0.1"
# endif

int main() {
	std::vector<int> portVector = {8080, 8081, 8082, 8083};
	Controller ctrl(portVector);
	for (auto &i : portVector)
		std::cout << i << std::endl;
    int fd;
	int rb;
	char buffer[10000];
	std::string response;
    ctrl.createSockets(AF_INET, SOCK_STREAM, 0, portVector, ADDR_TEST);
	while (1) {
		ctrl.acceptConnection(fd, 0);
		ctrl.acceptConnection(fd, 1);
		rb = read(fd, buffer, 10000);
		if (rb > 0) {
			std::cout << buffer << std::endl;
			response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 5\r\n"
            "\r\n"
            "OKMEN";
            send(fd, response.c_str(), response.length(), 0);
		}
		close (fd);
	}
}
