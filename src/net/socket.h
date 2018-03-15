#ifndef SIMPLE_SERVER_API_INCLUDE
#define SIMPLE_SERVER_API_INCLUDE

#include <stdint.h>
#include <string>
#include <memory>
#include <Buffer.h>

class SocketException final : public std::exception {
 public:
  SocketException(const char *what) : what_(what), errno_(0) {}
  SocketException(const char *what, int err)
      : what_(what), errno_(err) {}
  const char *what() const noexcept override;
 private:
  std::string what_;
 public:
  int errno_;
};

struct Socket {
  Socket(int fd) : fd_(fd) {};
  Socket(const Socket &);
  int fd_;
};

Socket *socket_make_server(const char *ip, uint16_t port);

Socket *socket_make_client();

void socket_close(Socket *socket);

bool socket_listen(Socket *server, uint32_t backlog);

Socket *socket_accept(Socket *server);

void socket_connect(Socket *socket, const char *ip, uint16_t port);

std::string socket_peer_ip(Socket *socket);

uint16_t socket_peer_port(Socket *socket);

int socket_make_nonblok(Socket *server);

size_t socket_read(Socket* socket, Buffer& buffer);

size_t socket_write(Socket* socket, const Buffer& buffer);

typedef std::shared_ptr<Socket> SocketSP;
typedef std::shared_ptr<Socket> SocketUP;
typedef std::weak_ptr<Socket> SocketWP;
#endif
