//
// Created by xiao shutong on 2018/3/14.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <net.h>
#include <fcntl.h>
using namespace std;

int make_nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags!=-1) {
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  } else {
    return flags;
  }
}

int main(int argc, const char *argv[])
{
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

  fd_set rfds;
  fd_set wfds;
  struct timeval tv;
  int retval;


  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_SET(fd, &rfds);
  FD_SET(fd, &wfds);

  tv.tv_sec = 5;
  tv.tv_usec = 0;

  retval = select(fd + 1, &rfds, &wfds, NULL, &tv);
  /* Don't rely on the value of tv now! */

  if (retval == -1)
    perror("select()");
  else if (retval) {
    printf("Data is available now.\n");
    /* FD_ISSET(0, &rfds) will be true. */
    if (FD_ISSET(fd, &rfds)) {
      cout << "socket readable" << endl;
    }
    if (FD_ISSET(fd, &wfds)) {
      cout << "socket writeable" << endl;
      int flag;
      int len = sizeof(flag);
      if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &flag, (socklen_t*)&len) != 0) {
        perror("getsockopt:");
      }
      cout << "getsockopt errno: " << flag << endl;
    }
  }
  else
    printf("No data within five seconds.\n");

  exit(EXIT_SUCCESS);
}