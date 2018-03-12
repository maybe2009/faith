//
// Created by xiao shutong on 2018/3/12.
//

#ifndef EPOLL_RIOWRAPPER_H
#define EPOLL_RIOWRAPPER_H

#include <rio.h>
#include <io_utility.h>
#include "rio.h"

namespace rio {
class ReaderWrapper final : public Reader  {
 public:
  explicit ReaderWrapper(int _fd) : fd(_fd), rp(nullptr) {
  }

  ssize_t readn(void *dst, size_t n) override {
    return rio_readn(fd, dst, n);
  }

  ssize_t readnb(void *dst, size_t n) override {
    if (rp == nullptr) {
      initBuffer();
    }
    return rio_bread(rp, dst, n);
  }

 private:
  void initBuffer() {
    rp = make_rio_t(fd);
  }
 private:
  int fd;
  rio_t* rp;
};

class WriterWrapper final : public Writer {
 public:
  explicit WriterWrapper(int _fd) : fd(_fd) {
  }

  ssize_t writen(const void *dst, size_t n) override {
    return rio_writen(fd, dst, n);
  }

 private:
  int fd;
};
}
#endif //EPOLL_RIOWRAPPER_H
