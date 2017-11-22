#include "simple_server_api.h"

#include <string>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

Socket::Socket(const Socket &rhs) : fd_(rhs.fd_) {}

//establish a tcp server binded with ip & port
Socket *socket_make_server(const char *ip, uint16_t port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd==-1) {
    printf("%s: create socket fail: %s", __func__, strerror(errno));
    return nullptr;
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &(address.sin_addr.s_addr))!=1) {
    printf("%s:parse ip:%s error:%s", __func__, ip, strerror(errno));
    return nullptr;
  }

  if (bind(fd, (struct sockaddr *) &address, sizeof(address))!=0) {
    printf("%s:bind fail:%s", __func__, strerror(errno));
    return nullptr;
  }

  return new Socket(fd);
}

bool socket_listen(Socket *server, uint32_t backlog) {
  return listen(server->fd_, backlog)==0 ? true : false;
}

Socket socket_accept(Socket *server) {
  return Socket(::accept(server->fd_, NULL, NULL));
}

std::string socket_peer_ip(Socket* socket) {
  struct sockaddr_in sa;
  uint32_t len = sizeof(sa);
  if (getpeername(socket->fd_, (struct sockaddr *)&sa, &len) == 0) {
    return std::string(inet_ntoa(sa.sin_addr));
  } else {
    return "";
  }
}

uint16_t socket_peer_port(Socket* socket) {
  struct sockaddr_in sa;
  uint32_t len = sizeof(sa);
  if (getpeername(socket->fd_, (struct sockaddr *)&sa, &len) == 0) {
    return ntohs(sa.sin_port);
  } else {
    return 0;
  }
}

int socket_make_nonblok(Socket *server) {
  int flags = fcntl(server->fd_, F_GETFL, 0);
  if (flags!=-1) {
    return fcntl(server->fd_, F_SETFL, flags | O_NONBLOCK);
  } else {
    return flags;
  }
}