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

void read_something(int fd) {
  ReaderWrapper rd(fd);

  ssize_t nread = 0;
  char buf[512];
  char *ptr = buf;
  int count = 20;
  do {
    nread = rd.readn(ptr++, 1);
    if (nread == 0) {
      cout << "peer send FIN" << endl;
    } else if (nread < 0) {
      perror("readn");
      return;
    }
  } while(nread > 0 && --count > 0);
  *ptr = 0;

  cout << "read " << buf << endl;
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

  int fd = net::socket();
  if (fd == -1) {
    perror("socket()");
    return -1;
  }

  if(net::make_nonblock(fd) == -1) {
    perror("make_nonblock()");
    return -1;
  }

  struct sockaddr_in addr;
  if (net::make_ipv4_addr(ip.c_str(), port, &addr) == -1) {
    perror("make_ipv4_addr");
    return -1;
  }

  if (net::connect(fd, (struct sockaddr*)&addr) == -1) {
    perror("connect()");
    cout << "coo " << errno << endl;
    if (errno != EINPROGRESS) {
      return -1;
    }
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
      printf("fd:%d, event:%u\n", events[i].data.fd, events[i].events);
    }
    sleep(5);
  }
}