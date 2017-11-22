#include "epoll.h"

Epoll::Epoll(int fd) : __fd(fd){};

Epoll::~Epoll() { close(__fd); };

int Epoll::control(int op, int fd, struct epoll_event *event)
{
    return epoll_ctl(__fd, op, fd, event);
}

int Epoll::select(struct epoll_event *events,
                  int maxevents,
                  int timeout)
{
    return epoll_wait(__fd, events, maxevents, timeout);
}

Epoll *make_epoll()
{
    int fd = epoll_create(1);
    if (fd == -1)
    {
        return nullptr;
    }
    return new Epoll(fd);
}