//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_TCPSERVER_H
#define EPOLL_TCPSERVER_H

#include <simple_server_api.h>
#include <Channel.h>
#include <Processor.h>
#include <io_utility.h>

#include <functional>
#include <string>
#include <map>
#include <iostream>

class Acceptor;
class TcpConnection;
class TcpServer;
class AcceptorWrapper;

typedef int TcpConnectionID;
typedef std::function<void(TcpConnection *)> TcpReadCallback;
typedef std::function<void(TcpConnection *)> TcpWriteCallback;
typedef std::function<void(Acceptor *)> TcpConnectCallback;
typedef std::function<void(AcceptorWrapper*)> AcceptorCallback;

/*!
 * @brief Acceptor在构造时，需要传入一个处于监听状态（调用listen后）的socket。将该socket
 * 的读事件注册到processor上，当读事件触发时，代表着有新的链接建立请求，就调用通过
 * setTcpConnectCallback设置的回调进行处理。同时Acceptor对外提供了accept方法建立链接，并
 * 返回新链接的socket
 */
class TcpAcceptor final {
 public:
  TcpAcceptor(Socket socket, Processor *processor);

  ~TcpAcceptor();

  void listen(int backlog);

  Socket accept();

  void setAcceptCallback(ReadCallback &callback);

 private:
  Socket socket_;
  ChannelPtr channel_;
};

class AcceptorWrapper final {
 public:
  AcceptorWrapper(const char *ip,
                  uint16_t port,
                  Processor *processor);

  ~AcceptorWrapper();

  void setAcceptorCallback(AcceptorCallback callback);

  void listen(int backlog);

  Socket accept();

 private:
  void onConnect(ChannelPtr channel);

 private:
  Socket *socket_;
  AcceptorCallback acceptorCallback_;
  TcpAcceptor *acceptor_;
};

class Acceptor : public Channel {
 public:
  /*!
   * @brief 构造函数
   * @param processor 回调处理器
   * @param server  处于监听状态（调用listen后）的socket
   */
  Acceptor(Processor *processor, Socket server);

  /*!
   * @brief 调用accept建立链接，返回新链接的socket
   * @return 新链接的socket
   */
  Socket accept();

  /*!
   * @brief 设置当有新的tcp连接时调用的callback
   * @param callback 回调函数
   */
  void setTcpConnectCallback(const TcpConnectCallback &callback);

  /*!
   * @brief 读事件代表着有新的链接建立请求过来
   */
  void react() override;

 private:
  Socket server_;
  TcpConnectCallback onConnectCallback_;
};

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
  explicit TcpServer(std::string ip, uint16_t port);

  /*!
   * @brief 监听指定的ip:port，并构造Acceptor对象，Acceptor对象会在指定的processor上
   * 注册监听事件
   * @param processor 指定的processor
   */
  void start(Processor *processor);

  /*!
   * @brief 设置链接建立时的回调
   * @param callback 链接建立时的回调
   */
  void setTcpConnectCallback(const TcpConnectCallback &callback);

 private:
  std::string ip_;
  uint16_t port_;
  Socket *server_;
  Acceptor *acceptor_;
  TcpConnectCallback onConnectCallback_;
  TcpAcceptor *tcp_acceptor_;
};

/*!
 * @brief 这个类是TCP链接的抽象，继承Reactor。通过Reactor的接口注册链接的读写事件，当相应
 * 事件触发时，会调用相应的回调进行处理。回调可以通过相关成员函数来设置。读/写操作也进行了封装，
 * 外部需要使用IOReader及其子类来完成相应的操作。这么做可以避免直接对外暴露socket fd。
 */
class TcpConnection : public Channel {
 public:
  TcpConnection(Processor *processor, const Socket &peer_);

  void react() override;

  /*!
   * @brief 获取读回调
   * @return 读回调
   */
  const TcpReadCallback &getTcpReadCallback() const;

  /*!
   * @brief 设置读回调
   * @param tcpReadCallback_
   */
  void setTcpReadCallback(const TcpReadCallback &tcpReadCallback_);

  const TcpWriteCallback &getTcpWriteCallback() const;

  void setTcpWriteCallback(const TcpWriteCallback &tcpWriteCallback);

  /*!
   * @brief 链接ID
   * @return 链接ID
   */
  TcpConnectionID getID() const;

  /*!
   * @brief read方法对外提供了对链接的读接口。只接受IOReader作为读者
   * @param reader 读者
   */
  void read(IOReader &reader);

  void write(IOWriter &writer);

 private:
  Socket peer_;
  TcpReadCallback tcpReadCallback_;
  TcpWriteCallback tcpWriteCallback_;
};

void DefaultTcpConnectedCb(Acceptor *acceptor);

void DefaultTcpConnectionCallback(Processor *prosessor,
                                  TcpServer *server,
                                  Socket peer);

void DefaultTcpConnectionReadCallback(TcpConnection *connection);

#endif //EPOLL_TCPSERVER_H
