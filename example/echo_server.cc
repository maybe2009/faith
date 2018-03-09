//
// Created by xiao shutong on 2017/11/16.
//

#include <Processor.h>
#include <EpollSelector.h>
#include <tcp_server.h>

using namespace std;

void print_peer(ChannelSP channel, SocketWP socket) {
  auto server_socket = socket.lock();
  auto new_socket = unique_ptr<Socket>(socket_accept(server_socket.get()));

  cout << "peer ip " << socket_peer_ip(new_socket.get()) << " port "
       << socket_peer_port(new_socket.get()) << endl;
}

int main(int argc, const char *argv[]) {
  try {
    auto epoll = unique_ptr<Selector>(new EpollSelector());

    auto processor = make_shared<Processor>(std::move(epoll));

    TcpServer server(processor, string("127.0.0.1"), 9527);
    server.setTcpConnectCallback(print_peer);
    server.start();

    cout << "process run" << endl;
    processor->process();
  } catch (SocketException &e) {
    cout << "Socket error " << e.what() << endl;
  }

  cout << "bye bye" << endl;

  return 0;
}
