//
// Created by xiao shutong on 2018/3/15.
//

#include <string>
#include <iostream>
#include <net.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <riowrapper.h>

using namespace std;

void wirte_something(int fd) {
  WriterWrapper wr(fd);
  if (wr.writen("Hello, World!", strlen("Hello, World!")) == -1) {
    perror("wirte_something:write");
    return;
  }
  cout << "write success" << endl;
}

int main(int argc, const char *argv[]) {
  std::string ip;
  uint16_t port;
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }


  ip = argv[1];
  port = atoi(argv[2]);

  struct sockaddr_in addr;
  if (net::make_ipv4_addr(ip.c_str(), port, &addr) == -1) {
    perror("make_ipv4_addr");
    return -1;
  }
  int fd = net::socket();
  if (fd < 0) {
    perror("socket");
    return -1;
  }
  if (net::make_nonblock(fd) == -1) {
    perror("make_nonblock");
    return -1;
  }
  if (net::bind(fd, (struct sockaddr*)&addr) == -1) {
    perror("bind");
    return -1;
  }
  if (net::listen(fd, 5) == -1) {
    perror("listen");
    return -1;
  }

  int epfd = epoll_create(1);
  if (epfd == -1) {
    perror("epoll_create");
    return -1;
  }

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLOUT | EPOLLET;
  event.data.fd = fd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) != 0) {
    perror("epoll_ctl");
  }

  while(1) {
    struct epoll_event events[10];
    int nfd = epoll_wait(epfd, events, 10, -1);
    if (nfd < 0) {
      perror("epoll_wait");
      return -1;
    }

    for (int i = 0; i < nfd; i++) {
      struct epoll_event ev = events[i];
      printf("fd:%d, event:%x\n", events[i].data.fd, events[i].events);

      if (events[i].events & EPOLLIN) {
        cout << "fd " << ev.data.fd << " is readable" << endl;
        int newfd = net::accept(ev.data.fd);
        if (newfd == -1) {
          perror("accept");
        }

        struct epoll_event new_event;
        new_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
        new_event.data.fd = newfd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &new_event) != 0) {
          perror("epoll_ctl");
        }
      }
    }
    sleep(5);
  }
}