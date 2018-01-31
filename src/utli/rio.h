//
// Created by xiao shutong on 2018/1/30.
//

#ifndef EPOLL_RIO_H
#define EPOLL_RIO_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

void unix_error(char *msg) /* unix-style error */
{
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(0);
}

//Only return with n bytes written to dst, or error
ssize_t rio_readn(int fd, void* dst, size_t n) {
  size_t nleft = n;
  char* buf = (char*)dst;

  while (nleft > 0) {
    ssize_t nread = read(fd, buf, nleft);
    if (nread <= 0) {
      if (0 == nread) {
        break;
      }
      else if (EINTR == nread || EAGAIN == nread || EWOULDBLOCK == nread) {
        nread = 0;
      }
      else {
        return -1;
      }
    }

    nleft = nleft - nread;
    buf = buf + nread;
  }

  return n - nleft;
}

ssize_t rio_writen(int fd, void* dst, size_t n) {
  size_t nleft = n;
  char* buf = (char*)dst;

  while (nleft > 0) {
    ssize_t  nwrite = write(fd, buf, nleft);
    if (nwrite < 0) {
      if (EINTR == nwrite || EAGAIN == nwrite || EWOULDBLOCK == nwrite) {
        nwrite = 0;
      }
      else {
        return -1;
      }
    }

    nleft = nleft - nwrite;
    buf = buf + nleft;
  }

  return n - nleft;
}

#define RIO_BUFFER_SIZE 8192
typedef struct {
  int fd;
  char buf[RIO_BUFFER_SIZE];
  char* buf_ptr;
  char* read_ptr;
} rio_t;

void rio_init(rio_t* rp, int fd) {
  rp->fd = fd;
  rp->buf_ptr = rp->buf;
  rp->read_ptr = rp->buf;
}

size_t rio_left(rio_t* rp) {
  return (rp->buf_ptr - rp->read_ptr);
}

ssize_t rio_bread(rio_t* rp, void* dst, size_t n) {
  //refill
  if (0 == rio_left(rp)) {
    ssize_t nread = rio_readn(rp->fd, rp->buf_ptr, RIO_BUFFER_SIZE);
    if (-1 == nread || 0 == nread) {
      return nread;
    }
    rp->buf_ptr += nread;
  }

  size_t min = n < rio_left(rp) ? n : rio_left(rp);
  memcpy(dst, rp->read_ptr, min);
  rp->read_ptr += min;
  return min;
}

ssize_t rio_breadline(rio_t* rp, void* dst, size_t max_len) {
  if (max_len < 1) {
    return 0;
  }

  size_t left = max_len;
  char* buf = (char*)dst;
  while(left-- > 0) {
    char c;
    ssize_t nread = rio_bread(rp, &c, 1);
    if (-1 == nread || 0 == nread) {
      return nread;
    }

    *buf++ = c;
    if ('\n' == c) {
      break;
    }
  }

  *buf = 0;
  return max_len - left;
}
#endif //EPOLL_RIO_H
