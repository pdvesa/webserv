#ifndef SERVER_HPP
#define SERVER_HPP

#include <ServerConfig.hpp>

class Server { //wrapper class for david
	private:
		ServerConfig	serverData;
		int				serverFD;
	public:
		Server(ServerConfig data, int fd);
		~Server();
		int getServerFD();
		ServerConfig &getServerData();
};

#endif