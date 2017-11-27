//
// Created by xiao shutong on 2017/11/17.
//

#include <tcp_server.h>

#include <functional>

Acceptor::Acceptor(Processor *processor, Socket server)
    : Channel(server.fd_, processor), server_(server) {}

Socket Acceptor::accept() {
  return socket_accept(&server_);
}

void Acceptor::setTcpConnectCallback(const TcpConnectCallback &callback) {
  onConnectCallback_ = callback;
}

void Acceptor::react() {
  if (canRead()) {
    onConnectCallback_(this);
  }
}

TcpServer::TcpServer(std::string ip, uint16_t port)
    : ip_(ip),
      port_(port),
      onConnectCallback_(DefaultTcpConnectedCb),
      tcp_acceptor_(nullptr) {}

void TcpServer::start(Processor *processor) {
  server_ = socket_make_server(ip_.c_str(), port_);
  socket_listen(server_, 5);

  acceptor_ = new Acceptor(processor, *server_);
  acceptor_->setTcpConnectCallback(onConnectCallback_);
  acceptor_->enableRead();

  auto another_server = socket_make_server(ip_.c_str(), port_ + 1);
  tcp_acceptor_ = new TcpAcceptor(*another_server, processor);
  tcp_acceptor_->listen(5);
}

void TcpServer::setTcpConnectCallback(const TcpConnectCallback &callback) {
  onConnectCallback_ = callback;
}

TcpConnection::TcpConnection(Processor *processor, const Socket &peer_)
    : Channel(peer_.fd_, processor), peer_(peer_) {}

void TcpConnection::react() {
  if (isClosed()) {
    std::cout << "peer connection closed" << std::endl;
    ::close(peer_.fd_);

    return;
  }

  if (canRead()) {
    tcpReadCallback_(this);
  }

  if (canWrite()) {
    tcpWriteCallback_(this);
  }
}

const TcpReadCallback &TcpConnection::getTcpReadCallback() const {
  return tcpReadCallback_;
}

void TcpConnection::setTcpReadCallback(const TcpReadCallback &tcpReadCallback_) {
  TcpConnection::tcpReadCallback_ = tcpReadCallback_;
}

void TcpConnection::read(IOReader &reader) {
  std::cout << "read at " << peer_.fd_ << std::endl;
  reader.read(peer_.fd_);
}

void TcpConnection::write(IOWriter &writer) {
  writer.write(peer_.fd_);
}

TcpConnectionID TcpConnection::getID() const {
  return peer_.fd_;
}

const TcpWriteCallback &TcpConnection::getTcpWriteCallback() const {
  return tcpWriteCallback_;
}

void TcpConnection::setTcpWriteCallback(const TcpWriteCallback &tcpWriteCallback) {
  tcpWriteCallback_ = tcpWriteCallback;
}

void DefaultTcpConnectedCb(Acceptor *acceptor) {
  Socket newSocket = acceptor->accept();
  std::cout << "Connection established. Peer ip " << socket_peer_ip
      (&newSocket) << " port " << socket_peer_port(&newSocket) << std::endl;
}

//void DefaultTcpConnectionCallback(Processor *prosessor,
//                                  TcpServer *server,
//                                  Socket peer) {
//  std::cout << "Default tcp connection callback" << std::endl;
//  //TODO use smart pointer to avoid memory leak
//  TcpConnection *new_conn = new TcpConnection(prosessor, peer);
//  new_conn->setTcpReadCallback(DefaultTcpConnectionReadCallback);
//
//  new_conn->enableRead();
//}

void DefaultTcpConnectionReadCallback(TcpConnection *connection) {

  Buffer buf(1024);
  BufferReader reader(buf);
  connection->read(reader);

  std::string msg(buf.seekHead(), buf.size());
  std::cout << "TcpConnection read content " << msg <<
            std::endl;
}

TcpAcceptor::TcpAcceptor(Socket socket, Processor *processor)
    : socket_(socket),
      channel_(std::make_shared<Channel>(socket.fd_, processor)) {
}

TcpAcceptor::~TcpAcceptor() {
}

void TcpAcceptor::setAcceptCallback(ReadCallback &callback) {
  channel_->setReadCallback(callback);
  channel_->enableRead();
}

void TcpAcceptor::listen(int backlog) {
  socket_listen(&socket_, backlog);
}

Socket TcpAcceptor::accept() {
  return socket_accept(&socket_);
}

AcceptorWrapper::AcceptorWrapper(const char *ip,
                                 uint16_t port,
                                 Processor *processor)
    : socket_(nullptr), acceptor_(nullptr) {
  socket_ = socket_make_server(ip, port);
  acceptor_ = new TcpAcceptor(socket_->fd_, processor);
}

AcceptorWrapper::~AcceptorWrapper() {
  delete acceptor_;
}

void AcceptorWrapper::listen(int backlog) {
  acceptor_->listen(backlog);

  ReadCallback read = std::bind(&AcceptorWrapper::onConnect, this,
                                std::placeholders::_1);
  acceptor_->setAcceptCallback(read);
}

void AcceptorWrapper::setAcceptorCallback(AcceptorCallback callback) {
  acceptorCallback_ = callback;
}

void AcceptorWrapper::onConnect(ChannelPtr channel) {
  acceptorCallback_(this);
}

Socket AcceptorWrapper::accept() {
  return acceptor_->accept();
}

