//
// Created by xiao shutong on 2017/12/11.
//

#include <RpcProtocol.h>
#include <socket.h>

#include <iostream>

using namespace std;
using namespace Rpc;

void handlerSimpleResponse(Socket* peer, const SimpleResponse& response) {

  cout << "result is " << response.c << endl;
}

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

    Buffer buf(8);
    SimpleRequest request;
    request.a = 1;
    request.b = 2;
    encode(request, buf);

    socket_write(&client, buf);

    buf.clear();
    socket_read(&client, buf);

    SimpleResponse response;
    decode(buf, response);
    handlerSimpleResponse(&client, response);

    socket_close(&client);
  } catch (SocketException &e) {
    cout << "socket exception " << e.what() << endl;
  } catch (BufferException &e) {
    cout << "buffer exception " << e.what() << endl;
  }

  return 0;
}