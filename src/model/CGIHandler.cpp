#include <CGIHandler.hpp>

CGI::CGI(Client &kunde, const HttpRequest &request, int pfd, std::string target) : client(kunde), req(request), path(target), env(), envp(), exitStatus(0), pollFD(pfd) {
	runCGI();
}

CGI::~CGI() {	
}

void CGI::runCGI() {
	pid_t	pid;
	int		pipes[2];
	char	*args[3];

	fillEnv();
	std::string temp = "/usr/bin/python3";
	if (path.rfind(".py") == path.size() -3) {
			args[0] = temp.data();
			args[1] = path.data();
	}
	else if (path.rfind(".cgi") == path.size() - 4) {
			args[0] = path.data();
			args[1] = nullptr;
	}
	args[2] = nullptr;
	if (pipe(pipes) == -1) {
		exitStatus = EXIT_FAILURE;
		return ;
	}
	pid = fork();
	if (pid == -1) {
		close(pipes[0]);
		close(pipes[1]);
		exitStatus = EXIT_FAILURE;
		return ;
	}
	if (pid == 0) {
		close(pipes[0]);
		if (dup2(pipes[1], STDOUT_FILENO) == -1)
			exit(1);
		execve(args[0], args, envp.data());
		perror("This happened: ");
		exit(EXIT_FAILURE);
	}
	else {
		close(pipes[1]);
		epollAdd(pollFD, pipes[0]);				
		client.setCgiFD(pipes[0]);		
		client.setPid(pid);
	} 
}

void CGI::fillEnv() { 
	std::string methods[4] = {"GET", "POST", "DELETE", "TEAPOT"};
	env.push_back("SERVER_SOFTWARE=KYS/0.0.1 (Unix)");
	env.push_back("SERVER_NAME=" + req.getServerConfig().getHost());
	env.push_back("SERVER_PORT=" + std::to_string(req.getServerConfig().getPort()));
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("METHOD=" + methods[req.getMethod()]);
	if (req.getMethod() == 1) {
		env.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
		env.push_back("CONTENT_LENGTH=" + req.getHeader("Content-Length"));
		std::vector temp = req.getBody().getContent();
		std::string	body(temp.begin(), temp.end());
		env.push_back("POST_BODY=" + body);
	}
	env.push_back("SCRIPT_NAME=" + req.getTarget()); 
	env.push_back("PATH_INFO=" + std::filesystem::current_path().string());
	for (const auto &var : env) {
		envp.push_back(const_cast<char*>(var.c_str()));
	}
	envp.push_back(nullptr);
}
