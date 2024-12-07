#include <CGIHandler.hpp>

static pid_t childPID = -1;

static void alarmHandler(int signal) {
	if (childPID != -1 && signal == SIGALRM)
		kill(childPID, SIGTERM);
}

CGI::CGI(HttpRequest request) : req(request), env(), envp(), exitStatus(0) {
	runCGI();
}

CGI::~CGI() {	
}

void CGI::runCGI() {
	pid_t	pid;
	int		pipes[2];
	char	*args[3];

	fillEnv();
	std::string	script = req.getPath();
	std::string temp = "/usr/bin/python3";
	switch (req.getCGIStatus()) {
		case PY:
			args[0] = temp.data();
			args[1] = script.data();
			break ;
		case CGI_E: 
			args[0] = script.data();
			args[1] = nullptr;
			break ;
		default:
			std::cout << "This should never happen, segfaulting next... :)" << std::endl;
	}
	args[2] = nullptr;
	if (pipe(pipes) == -1) {
		exitStatus = 1;
		return ;
	}
	pid = fork();
	if (pid == -1) {
		close(pipes[0]);
		close(pipes[1]);
		exitStatus = 1;
		return ;
	}
	if (pid == 0) {
        close(pipes[0]);
		if (dup2(pipes[1], STDOUT_FILENO) == -1)
			exit(1) ;
		close(pipes[1]);
		execve(args[0], args, envp.data());
		perror("This happened: ");
		exit(1); //temp handlers
	}
	else {
		int status;
		int rv;
		childPID = pid;
		signal(SIGALRM, alarmHandler);
		alarm(15);
		std::vector<unsigned char>	buffer(BUF_SIZE);
		close(pipes[1]);
		while ((rv = read(pipes[0], buffer.data(), BUF_SIZE)) > 0) {
			buffer.resize(rv);
			cgiResponse.append(buffer.begin(), buffer.end());
			if (rv < BUF_SIZE) 
				break;
		}
		close(pipes[0]);
		waitpid(pid, &status, 0);
		alarm(0);
		if (WIFEXITED(status))
            exitStatus = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exitStatus = WTERMSIG(status);
	} 
}

void CGI::fillEnv() { //debug this shit if these are even correct like getHost
	env.push_back("SERVER_SOFTWARE=KYS/0.0.1 (Unix)");
	env.push_back("SERVER_NAME=" + req.getServer().getHost());
	env.push_back("SERVER_PORT=" + std::to_string(req.getServer().getPort()));
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("METHOD=" + req.getMethod());
	env.push_back("CONTENT_TYPE=" + req.getMapValue("Content-Type"));
	env.push_back("CONTENT_LENGTH=" + req.getMapValue("Content-Length"));
//	env.push_back("QUERY_STRING="); //dont know if needed
	env.push_back("SCRIPT_NAME=" + req.getPath()); 
	env.push_back("PATH_INFO=" + std::filesystem::current_path().string()); // + req.getPath()); //not sure what goes here
//	env.push_back("REMOTE_ADDR="); //dont know if needed (client addr) i dont think we even save it anymore :)
	std::vector temp = req.getBody();
	std::string	body(temp.begin(), temp.end());
	env.push_back("POST_BODY=" + body); //lets hope that converting doessnt break anything
	for (const auto &var : env) {
		envp.push_back(const_cast<char*>(var.c_str())); //make sure you dont die
	}
	envp.push_back(nullptr);
}
