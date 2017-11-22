//
// Created by DW on 2017/11/17.
//

#ifndef EPOLL_ENUMS_H
#define EPOLL_ENUMS_H

struct EnumSelectorEvent {
  enum Event{
    ON_READ = 1 << 0,
    ON_WRITE = 1 << 1,
    ON_CONNECT = 1 << 2,
    ON_CLOSE = 1 << 3,
  };
};

struct EnumSelectorOption {
  enum Option {
    ADD = 1,
    MOD = 2,
    DEL = 3,
  };
};

#endif //EPOLL_ENUMS_H
