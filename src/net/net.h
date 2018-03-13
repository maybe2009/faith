//
// Created by tombkeeper on 2018/3/11.
//

#ifndef EPOLL_NET_H
#define EPOLL_NET_H

#include <string>
#include "../utli/riowrapper.h"
#include <socket.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace rio;

namespace net {
class TcpConn final {
 public:
  explicit TcpConn(int _fd) : fd(_fd), reader(_fd), writer(_fd){
  }

  void shutdownWrite() {
    if (shutdown(fd, SHUT_WR) != 0) {
      std::cerr << "shutdown error" << std::endl;
    }
  }

  void shutdownRead() {
    if (shutdown(fd, SHUT_RD) != 0) {
      std::cerr << "shutdown error " << errno << std::endl;
    }
  }

  void tcpNoDelay() {
    int flag = 1;
    int result = setsockopt(fd,            /* socket affected */
                            IPPROTO_TCP,     /* set option at TCP level */
                            TCP_NODELAY,     /* name of option */
                            (char *) &flag,  /* the cast is historical
                                                         cruft */
                            sizeof(int));    /* length of option value */

    if (result != 0) {
      std::cerr << "setsockopt error " << errno << std::endl;
    }
  }

  void close() {
    ::close(fd);
  }



 private:
  int fd;
 public:
  class ReaderWrapper reader;
  class WriterWrapper writer;
};

class TcpListener final {
 public:
  TcpListener(int _fd) : fd(_fd) {

  }

  TcpConn* Accept() {
    Socket server = Socket(fd);
    Socket* s = socket_accept(&server);
    return new TcpConn(s->fd_);
  }

  ~TcpListener() {
    ::close(fd);
  }

 private:
  int fd;
  Socket* s;
};

TcpListener* TcpListen(const char *ip, uint16_t port) {
  Socket* s = socket_make_server(ip, port);
  socket_listen(s, 5);
  return new TcpListener(s->fd_);
}

TcpConn* TcpDial(const char* ip, uint16_t port) {
  Socket* s = socket_make_client();
  socket_connect(s, ip, port);
  return new TcpConn(s->fd_);
}

};

#endif //EPOLL_NET_H
