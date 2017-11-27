//
// Created by xiao shutong on 2017/11/27.
//

#include <Processor.h>
#include <tcp_server.h>
#include <EpollSelector.h>
#include "telnet_im_protocol.h"

#include <iostream>
#include "telnet_im_protocol.h"

using namespace std;

map<int, ChannelPtr> fd_channel_map;
map<string, ChannelPtr> name_channel_map;

void im_on_connection_closed(ChannelPtr channel) {
  std::cout << "peer connection close" << std::endl;
  channel->close();
}

void im_on_write(ChannelPtr channel, std::string msg) {
  BufferWriter writer(100);
  writer.load(msg.c_str(), msg.size());

  channel->write(writer);
  channel->disableWrite();
}

void im_on_user_data(ChannelPtr channel) {
  Buffer buf(100);
  BufferReader reader(buf);
  channel->read(reader);
  string msg(buf.seekHead(), buf.size());
  cout << "im_on_user_data " << msg << endl;

  TelnetIMDecoder decoder;
  IMUserMsg user_msg;
  try {
    decoder.unmarshal(buf.getReader(), user_msg);
    std::cout << user_msg.from << " speak to " << user_msg.to << " "
              << user_msg.msg <<
              std::endl;
    name_channel_map.insert(make_pair(user_msg.from, channel));

    auto peer = name_channel_map.find(user_msg.to);
    if (name_channel_map.end()!=peer) {
      ChannelPtr peer_channel = peer->second;
      peer_channel->setWriteCallback(std::bind(im_on_write,
                                               std::placeholders::_1,
                                               user_msg.msg));
      peer_channel->enableWrite();
    } else {
      cout << user_msg.to << " offline" << endl;
    }
  } catch (DecodeException &e) {
    cout << "exception occured " << e.what() << endl;
  }
}

void im_on_user_connect(AcceptorWrapper *acceptorWrapper,
                        Processor *processor) {
  Socket socket = acceptorWrapper->accept();
  std::cout << "NEW Connection established. Peer ip " << socket_peer_ip
      (&socket) << " port " << socket_peer_port(&socket) << std::endl;
  auto channel = std::make_shared<Channel>(socket.fd_, processor);
  fd_channel_map.insert(make_pair(socket.fd_, channel));

  channel->setReadCallback(im_on_user_data);
  channel->setCloseCallback(im_on_connection_closed);
  channel->enableRead();
}

int main(int argc, const char *argv[]) {
  Processor *processor = new Processor(new EpollSelector());
  AcceptorWrapper acceptor("127.0.0.1", 7758, processor);
  acceptor.setAcceptorCallback(std::bind(im_on_user_connect,
                                         std::placeholders::_1, processor));

  acceptor.listen(5);

  while (1) {
    cout << "process run" << endl;
    processor->process();
    sleep(1);
  }
  delete processor;
  cout << "bye bye" << endl;

  return 0;
}