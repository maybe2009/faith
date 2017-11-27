//
// Created by xiao shutong on 2017/11/22.
//

#ifndef EPOLL_TELNET_IM_PROTOCOL_H
#define EPOLL_TELNET_IM_PROTOCOL_H

#include <Buffer.h>
#include <string>
#include <vector>
#include <exception>
struct IMUserMsg {
  std::string from;
  std::string to;
  std::string msg;
};

class DecodeException : public std::exception {
 public:
  explicit DecodeException(const char* info) : std::exception(), what_(info) {}

  const char *what() const noexcept override {
    return what_.c_str();
  }

 private:
  std::string what_;
};

class TelnetIMDecoder {
 public:
  void unmarshal(BinaryReader &&in, IMUserMsg &out) {
    in.skipWhiteSpace();

    char ch;

    in.pop(&ch);
    if ('#' == ch) {
      char buf[10];
      memset(buf, 0, 10);
      size_t  len = in.popUntil(buf, 10, '#');
      in.pop();
      std::string from(buf, len);
      out.from = from;
    } else {
      throw DecodeException("parse from, # needed");
    }

    in.skipWhiteSpace();

    in.pop(&ch);
    if ('#' == ch) {
      char buf[10];
      memset(buf, 0, 10);
      size_t  len = in.popUntil(buf, 10, '#');
      in.pop();
      std::string to(buf, len);
      out.to = to;
    } else {
      throw DecodeException("parse to, # needed");
    }

    in.skipWhiteSpace();

    in.pop(&ch);
    if ('#' == ch) {
      char buf[100];
      memset(buf, 0, 100);
      size_t  len = in.popUntil(buf, 100, '#');
      in.pop();
      std::string msg(buf, len);
      out.msg = msg;
    } else {
      throw DecodeException("parse msg, # needed");
    }
  }
};
#endif //EPOLL_TELNET_IM_PROTOCOL_H
