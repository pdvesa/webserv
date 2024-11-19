#ifndef EPOLLUTILS_HPP
#define EPOLLUTILS_HPP

#include <sys/epoll.h>
#include <fcntl.h>
#include <fstream>

void	epollAdd(int epollFD, int addFD, bool in);
void	epollDelete(int epollFD, int deleteFD);
void	epollModify(int epollFD, int modFD);
void	setNonBlocking(int fd);

#endif