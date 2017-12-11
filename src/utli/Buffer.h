#ifndef __FAITH_SOCKET_BUFFER__
#define __FAITH_SOCKET_BUFFER__

#include <unistd.h>
#include <string.h>
#include <map>

using std::map;
using std::make_pair;

class BufferException : public std::exception {
 public:
  explicit BufferException(const char *what) : exception(), what_(what) {
  }

  const char *what() const noexcept override {
    return what_.c_str();
  }

 private:
  std::string what_;
};

class BinaryReader {
 public:
  BinaryReader(void *src, size_t len) :
      binary_(static_cast<char *>(src)),
      len_(len),
      cur_index_(0),
      remain_size_(len) {}

  size_t pop() {
    if (remain_size_ < 1) {
      return 0;
    }

    cur_index_++;
    remain_size_--;
    return 1;
  }

  template<typename T>
  size_t pop(T *dst) {
    size_t type_size = sizeof(T);
    if (type_size > remain_size_) {
      return 0;
    }

    *dst = *const_cast<T *>(binary_ + cur_index_);
    cur_index_ += sizeof(T);
    remain_size_ -= sizeof(T);
    return sizeof(T);
  }

  size_t pop(void *dst, size_t size) {
    if (size > remain_size_) {
      return 0;
    }

    memcpy(dst, binary_, size);
    cur_index_ += size;
    remain_size_ -= size;
    return size;
  }

  size_t popUntil(void *dst, size_t max_len, char v) {
    size_t end_idx = cur_index_;
    for (; end_idx < len_; end_idx++) {
      if (v==*(binary_ + end_idx)) {
        break;
      }
    }

    if (end_idx==len_) {
      return 0;
    }

    size_t count = end_idx - cur_index_ + 1;
    //if count is 0, the first byte is v. Otherwise, omit last byte(v itself)
    if (0==count) {
      return 0;
    } else {
      count--;
      count = count > max_len ? max_len : count;
      memcpy(dst, binary_ + cur_index_, count);
    }

    cur_index_ += count;
    remain_size_ -= count;

    return count;
  }

  size_t skipWhiteSpace() {
    size_t count = 0;
    for (; cur_index_ < len_; cur_index_++, remain_size_--, count++) {
      if (!isspace(binary_[cur_index_])) {
        return count;
      }
    }
  }

 private:
  const char *const binary_;
  size_t len_;
  size_t cur_index_;
  size_t remain_size_;
};

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

  uint64_t readFromFd(int fd, int *err) {
    ssize_t nread = ::read(fd, seekCurrent(), freeSize());
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

  template<typename T>
  size_t pop(T *dst) {
    if (sizeof(T) > freeSize_) {
      throw BufferException("expected pop size is larger then free size of "
                                "buffer");
    }

    memcpy(dst, buffer_ + curIndex_, sizeof(T));
    curIndex_ += sizeof(T);
    freeSize_ -= sizeof(T);

    return sizeof(T);
  }

  template<typename T>
  size_t push(T *src) {
    if (sizeof(T) > freeSize_) {
      throw BufferException("expected push size is larger then free size of "
                                "buffer");
    }

    memcpy(buffer_ + curIndex_, src, sizeof(T));

    return sizeof(T);
  }

  BinaryReader getReader() const {
    return BinaryReader(buffer_, capacity_);
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
#endif