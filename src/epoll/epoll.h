#ifndef __LETTER_EPOLL__
#define __LETTER_EPOLL__

#include <sys/epoll.h>
#include <unistd.h>

struct Epoll
{
    Epoll(int fd);

    ~Epoll();

    int control(int op, int fd, struct epoll_event *event);

    int select(struct epoll_event *events,
               int maxevents,
               int timeout);

    int __fd;
};

Epoll *make_epoll();

#endif