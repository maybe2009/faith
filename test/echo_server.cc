//
// Created by xiao shutong on 2017/11/16.
//

#include <Processor.h>
#include <EpollSelector.h>
#include <tcp_server.h>

#include <string>
#include <iostream>
#include <map>
#include "telnet_im_protocol.h"

using namespace std;

map<int, Socket *> im_sockets;
map<int, TcpConnection *> im_connections;

std::string msg;

void im_on_out(TcpConnection *connection, std::string msg) {
  std::cout << "write " << msg << std::endl;
  BufferWriter writer(0);
  writer.load(msg.c_str(), msg.size());

  connection->write(writer);
  connection->disableWrite();
}

void im_on_data(TcpConnection *connection) {
  Buffer buf(100);
  BufferReader reader(buf);
  connection->read(reader);

  std::string msg(buf.seekHead(), buf.size());
  std::cout << "im_read_data " << msg << std::endl;

  TelnetIMDecoder decoder;
  IMUserMsg user_msg;
  try {
    decoder.unmarshal(buf.getReader(), user_msg);
    std::cout << user_msg.from << " speak to " << user_msg.to << " "
              << user_msg.msg <<
              std::endl;
  } catch (DecodeException &e) {
    std::cout << "exception occurred " << e.what() << std::endl;
  }
}

void im_login_connect(Processor *processor, Acceptor *acceptor) {
  auto new_socket = new Socket(acceptor->accept());
  im_sockets.insert(make_pair(new_socket->fd_, new_socket));

  auto connection = new TcpConnection(processor, *new_socket);
  im_connections.insert(make_pair(new_socket->fd_, connection));

  connection->setTcpReadCallback(im_on_data);
  connection->wannaRead();
}

int main(int argc, const char *argv[]) {
  Processor *processor = new Processor(new EpollSelector());
  TcpServer server(string("127.0.0.1"), 9527);
  server.setTcpConnectCallback(std::bind(im_login_connect, processor,
                                         std::placeholders::_1));
  server.start(processor);

  while (1) {
    cout << "process run" << endl;
    processor->process();
    sleep(1);
  }
  delete processor;
  cout << "bye bye" << endl;

  return 0;
}
