//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_TCPSERVER_H
#define EPOLL_TCPSERVER_H

#include <socket.h>
#include <Channel.h>
#include <Processor.h>
#include <io_utility.h>

#include <functional>
#include <string>
#include <map>
#include <iostream>

typedef std::function<void(ChannelSP, SocketWP)> TcpOnConnectCallback;
/*!
 * @brief TcpServer在指定的ip:port上监听，并且将监听事件通过Acceptor注册，当链接建立时，
 * 会调用Acceptor的回调函数处理。回调事件通过setTcpConnectCallback接口设置
 */
class TcpServer {
 public:
  /*!
   * @brief 构造函数
   * @param ip 监听的ip
   * @param port 监听的端口
   */
  TcpServer(ProcessorSP processor, std::string ip, uint16_t port);

  /*!
   * @brief 监听指定的ip:port，并构造Acceptor对象，Acceptor对象会在指定的processor上
   * 注册监听事件
   * @param processor 指定的processor
   */
  void start();

  /*!
   * @brief 设置链接建立时的回调
   * @param callback 链接建立时的回调
   */
  void setTcpConnectCallback(const TcpOnConnectCallback &callback);

 private:
  ProcessorSP processor_;
  std::string ip_;
  uint16_t port_;
  SocketSP server_;
  ChannelSP listen_channel_;
  TcpOnConnectCallback onConnectCallback_;
};

class TcpConnection final {
 public:
  explicit TcpConnection(SocketUP socket) : socket_(socket) {}

 private:
  SocketUP socket_;
};
#endif //EPOLL_TCPSERVER_H
