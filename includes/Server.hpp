#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <ServerConfig.hpp>

class Server {
	private:
		ServerConfig					serverData;
		std::shared_ptr<ServerConfig>	configPtr; 
		int								serverFD;
	public:
		Server(ServerConfig data, int fd);
		~Server();
		int getServerFD() const;
		ServerConfig getServerData() const;
		std::shared_ptr<ServerConfig> getServerPtr() const;
};

#endif