//
// Created by xiao shutong on 2017/11/17.
//

#include <tcp_server.h>

#include <functional>
#include <memory>

using std::make_shared;
using std::weak_ptr;
using std::shared_ptr;

TcpServer::TcpServer(ProcessorSP processor, std::string ip, uint16_t port)
    : processor_(processor),
      ip_(ip),
      port_(port) {}

void TcpServer::start() {
  server_ = shared_ptr<Socket>(socket_make_server(ip_.c_str(), port_));

  listen_channel_ = make_shared<Channel>(server_->fd_, processor_);
  listen_channel_->setReadCallback(std::bind(onConnectCallback_,
                                             std::placeholders::_1, server_));
  listen_channel_->enableRead();

  socket_listen(server_.get(), 5);
}

void TcpServer::setTcpConnectCallback(const TcpOnConnectCallback &callback) {
  onConnectCallback_ = callback;
}




