//
// Created by xiao shutong on 2017/12/11.
//

#ifndef EPOLL_RPCPROTOCOL_H
#define EPOLL_RPCPROTOCOL_H

#include <Buffer.h>

namespace Rpc {
class RpcProtocolBase {
 public:
  virtual void operator<<(Buffer &buf) = 0;

  virtual void operator>>(Buffer &buf) const = 0;
};

class SimpleRequest : public RpcProtocolBase {
 public:
  void operator<<(Buffer &buf) override;

  void operator>>(Buffer &buf) const override;

 public:
  int a;
  int b;
};

class SimpleResponse : public RpcProtocolBase {
 public:
  void operator<<(Buffer &buf) override;

  void operator>>(Buffer &buf) const override;

 public:
  int c;
};

void encode(const RpcProtocolBase &in, Buffer &out);

void decode(Buffer &in, RpcProtocolBase &out);
};
#endif //EPOLL_RPCPROTOCOL_H
