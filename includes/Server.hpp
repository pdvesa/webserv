#ifndef SERVER_HPP
#define SERVER_HPP

#include <ServerConfig.hpp>

class Server {
	private:
		ServerConfig	serverData;
		int				serverFD;
	public:
		Server(ServerConfig data, int fd);
		~Server();
		int getServerFD() const;
		ServerConfig getServerData() const;
};

#endif