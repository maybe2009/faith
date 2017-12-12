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
                          push_index_(0),
                          pop_index_(0) {
    buffer_ = (BYTE *) malloc(sizeof(BYTE)*capacity_);
    popPos_ = buffer_;
  }

  uint64_t write(const void *buf, uint64_t size) {
    uint64_t nwrite = size > freeSize() ? freeSize() : size;
    memcpy(buffer_ + push_index_, buf, nwrite);
    push_index_ += nwrite;
    return nwrite;
  }

  uint64_t readFromFd(int fd, int *err) {
    *err = 0;
    ssize_t nread = ::read(fd, current(), freeSize());
    if (nread < 0) {
      *err = errno;
      return 0;
    } else {
      push_index_ += nread;
      return nread;
    }
  }

  void clear() {
    push_index_ = 0;
    pop_index_ = 0;
    popPos_ = buffer_;
  }

  BYTE *seek(uint64_t offset) const {
    offset = offset > capacity_ ? capacity_ : offset;
    return buffer_ + offset;
  }

  BYTE *head() const {
    return seek(0);
  }

  BYTE *current() const {
    return seek(push_index_);
  }

  uint64_t freeSize() const {
    return capacity_ - push_index_;
  }

  uint64_t size() const {
    return push_index_;
  }

  template<typename T>
  size_t pop(T *dst) {
    if (sizeof(T) > freeSize()) {
      throw BufferException("expected pop size is larger then free size of "
                                "buffer");
    }
//    for (auto i = 0; i < sizeof(T); i++) {
//      printf("%x ", *(buffer_+i));
//    }
//    printf("curIndex is %u, size if %u\n", push_index_, sizeof(T));
    memcpy(dst, buffer_ + pop_index_, sizeof(T));
    pop_index_ += sizeof(T);
    return sizeof(T);
  }

  template<typename T>
  size_t push(T *src) {
    printf("push %u\n", freeSize());
    if (sizeof(T) > freeSize()) {
      throw BufferException("expected push size is larger then free size of "
                                "buffer");
    }

    memcpy(buffer_ + push_index_, src, sizeof(T));
    push_index_ += sizeof(T);
    return sizeof(T);
  }

  BinaryReader getReader() const {
    return BinaryReader(buffer_, capacity_);
  }

  void print() {
    for (uint64_t i = 0; i < push_index_; i++) {
      printf("%x ", *(buffer_+i));
    }
  }

  ~Buffer() {
    free(buffer_);
  }

 private:
  uint64_t capacity_;
  uint64_t push_index_;
  uint64_t pop_index_;
  BYTE *buffer_;
  BYTE *popPos_;
};
#endif