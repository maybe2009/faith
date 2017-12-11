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
    Socket client = *socket_make_client();
    cout << "ip " << ip << " port " << port << endl;
    socket_connect(&client, ip.c_str(), port);
    cout << "connect to " << socket_peer_ip(&client) << ":"
         << socket_peer_port(&client) << endl;

    Buffer buf(10);
    buf.write("Lily", sizeof("Lily"));
    socket_write(&client, buf);

    socket_close(&client);
  } catch (SocketException &e) {
    cout << e.what() << endl;
  } catch (BufferException &e) {
    cout << e.what() << endl;
  }

  return 0;
}