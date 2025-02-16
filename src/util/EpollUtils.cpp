#include <EpollUtils.hpp>
#include <iostream> //debug

void epollAdd(int epollFD, int addFD) {
	epoll_event	temp;
	temp.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP;
	temp.data.fd = addFD;
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, addFD, &temp) == -1) 
		throw std::runtime_error("Adding FD to epoll failed");
	setNonBlocking(addFD);
//	std::cout << "Added to epoll from " << addFD << std::endl;
}

void epollDelete(int epollFD, int deleteFD) {
	epoll_event	temp;
	temp.data.fd = deleteFD;
	if (epoll_ctl(epollFD, EPOLL_CTL_DEL, deleteFD, &temp) == -1) 
		throw std::runtime_error("Deleting FD from epoll failed");
//	std::cout << "Deleted epoll from " << deleteFD << std::endl; 
}

void epollModify(int epollFD, int modFD) {
	epoll_event	temp;
	temp.events = EPOLLOUT | EPOLLRDHUP;
	temp.data.fd = modFD;
	if (epoll_ctl(epollFD, EPOLL_CTL_MOD, modFD, &temp) == -1) 
		throw std::runtime_error("Modifying FD from epoll failed");	
	setNonBlocking(modFD);
//	std::cout << "Modified epoll from " << modFD << std::endl;
}

void setNonBlocking(int fd) {
	int	flags;
	if ((flags = fcntl(fd, F_GETFL)) == -1)
		throw std::runtime_error("FCUNT CAN'T GET THE FLAGS");
	flags = (flags | O_NONBLOCK);
	if (fcntl(fd, F_SETFL, flags) == -1)
		throw std::runtime_error("FCUNT CAN'T SET THE FLAGS");
} //loaned straight from stackoverflow
