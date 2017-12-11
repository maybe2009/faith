//
// Created by xiao shutong on 2017/12/11.
//

#include "RpcProtocol.h"

using namespace Rpc;

void SimpleRequest::operator<<(Buffer &buf) {
  buf.pop(&a);
  buf.pop(&b);
}
void SimpleRequest::operator>>(Buffer &buf) const {
  buf.push(&a);
  buf.push(&b);
}
void encode(const RpcProtocolBase &in, Buffer &out) {
  in >> out;
}
void decode(Buffer &in, RpcProtocolBase &out) {
  out << in;
}
void SimpleResponse::operator<<(Buffer &buf) {
  buf.pop(&c);
}
void SimpleResponse::operator>>(Buffer &buf) const {
  buf.push(&c);
}
