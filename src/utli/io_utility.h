//
// Created by DW on 2017/11/17.
//

#ifndef EPOLL_IOUTILITY_H
#define EPOLL_IOUTILITY_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string>

#include <Buffer.h>

class IOReader {
 public:
  virtual uint64_t read(int fd) = 0;
};

class IOWriter {
 public:
  virtual uint64_t write(int fd) = 0;
};

class BufferReader : public IOReader {
 public:
  BufferReader(uint64_t buf_size) : buf_(buf_size) {}

  uint64_t read(int fd) override {
    if (0 == buf_.freeSize()) {
      printf("buffer overflow\n");
      return 0;
    }

    int err = 0;
    printf("buffer reader read %lu\n", buf_.readFromFd(fd, &err));

    if (err != 0) {
      printf("error occurred err %d\n", err);
    }
  }

  std::string getBufData() {
    return std::string(buf_.seekHead(), buf_.size());
  }

  void clear() {
    buf_.clear();
  }

 private:
  Buffer buf_;
};

class BufferWriter : public IOWriter {
 public:
  BufferWriter(uint64_t buf_size) : buf_(buf_size) {}

  uint64_t load(const void* src, uint64_t size) {
    uint64_t real_size = size > buf_.freeSize() ? buf_.freeSize() : size;
    buf_.write(src, real_size);
  }

  uint64_t write(int fd) override {
    int nwrite = ::write(fd, buf_.seekHead(), buf_.size());
    if (nwrite <0) {
      printf("writer error\n");
      return 0;
    }
  }

  void clear() {
    buf_.clear();
  }
 private:
  Buffer buf_;
};
#endif //EPOLL_IOUTILITY_H
