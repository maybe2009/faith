//
// Created by xiao shutong on 2017/12/11.
//

#include <RpcProtocol.h>
#include <socket.h>

#include <iostream>

using namespace std;
int main(int argc, const char *argv[]) {
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  try {
    Socket server = *socket_make_server(ip.c_str(), port);
    socket_listen(&server, 5);
    Socket new_socket = *socket_accept(&server);
    cout << "connection established ip" << ip << ":" << port << endl;
    Buffer buf(10);
    socket_read(&new_socket, buf);
    string content(buf.seekHead(), buf.size());
    cout << "content is " << content << endl;

    socket_close(&new_socket);
    socket_close(&server);
  } catch (SocketException &e) {
    cout << e.what() << endl;
  } catch (BufferException &e) {
    cout << e.what() << endl;
  }

  return 0;
}