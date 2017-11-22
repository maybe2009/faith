#ifndef __FAITH_SOCKET_BUFFER__
#define __FAITH_SOCKET_BUFFER__

#include <unistd.h>
#include <string.h>
#include <map>

using std::map;
using std::make_pair;

class Buffer {
  typedef char BYTE;

 public:
  Buffer(uint64_t size) : capacity_(size),
                          curIndex_(0),
                          freeSize_(size) {
    buffer_ = (BYTE *) malloc(sizeof(BYTE)*capacity_);
    popPos_ = buffer_;
  }

  uint64_t write(const void *buf, uint64_t size) {
    uint64_t nwrite = size > freeSize_ ? freeSize_ : size;
    memcpy(buffer_ + curIndex_, buf, nwrite);

    curIndex_ += nwrite;
    freeSize_ -= nwrite;

    return nwrite;
  }

  uint64_t readFromFd(int fd, int* err) {
    ssize_t  nread = ::read(fd, seekCurrent(), freeSize());
    if (nread < 0) {
      *err = errno;
      return 0;
    } else {
      curIndex_ += nread;
      freeSize_ -= nread;
      return nread;
    }
  }

  void clear() {
    curIndex_ = 0;
    freeSize_ = capacity_;
    popPos_ = buffer_;
  }

  BYTE *seek(uint64_t offset) const {
    offset = offset > capacity_ ? capacity_ : offset;
    return buffer_ + offset;
  }

  BYTE *seekHead() const {
    return seek(0);
  }

  BYTE *seekCurrent() const {
    return seek(curIndex_);
  }

  uint64_t freeSize() const {
    return freeSize_;
  }

  uint64_t size() const {
    return curIndex_;
  }

  uint8_t popUint8() {
    uint8_t *ptr = (uint8_t *) popPos_;
    popPos_ = (BYTE *) ((uint8_t *) popPos_ + 1);
    return *ptr;
  }

  bool popN(void *dst, uint64_t n) {
    if (popPos_ + n > buffer_ + capacity_) {
      return false;
    } else {
      memcpy(dst, popPos_, n);
      popPos_ += n;
      return true;
    }
  }

  ~Buffer() {
    free(buffer_);
  }

 private:
  uint64_t capacity_;
  uint64_t curIndex_;
  uint64_t freeSize_;
  BYTE *buffer_;
  BYTE *popPos_;
};

class SocketBuffer {
 public:
  Buffer *getReadBuffer(int fd) const {
    return socketReadBuffer_.at(fd);
  }

  Buffer *getWriteBuffer(int fd) const {
    return socketWriteBuffer_.at(fd);
  }

  void allocateBufferForSocket(int fd, uint64_t size) {
    Buffer *rdBuf = new Buffer(size);
    socketReadBuffer_.insert(make_pair(fd, rdBuf));

    Buffer *wrBuf = new Buffer(size);
    socketWriteBuffer_.insert(make_pair(fd, wrBuf));
  }

  ~SocketBuffer() {
    for (auto buf : socketReadBuffer_) {
      delete buf.second;
    }

    for (auto buf : socketWriteBuffer_) {
      delete buf.second;
    }
  }

 private:
  map<int, Buffer *> socketReadBuffer_;
  map<int, Buffer *> socketWriteBuffer_;
};
#endif