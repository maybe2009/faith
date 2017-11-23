#ifndef SIMPLE_SERVER_API_INCLUDE
#define SIMPLE_SERVER_API_INCLUDE
#include <stdint.h>
#include <string>

struct Socket
{
  Socket(int fd) : fd_(fd){};
  Socket(const Socket&);
  int fd_;
};

Socket *socket_make_server(const char *ip, uint16_t port);

bool socket_listen(Socket *server, uint32_t backlog);

Socket socket_accept(Socket *server);

std::string socket_peer_ip(Socket* socket);

uint16_t socket_peer_port(Socket* socket);

int socket_make_nonblok(Socket *server);

#endif
