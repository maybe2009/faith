//
// Created by DW on 2017/11/16.
//

#ifndef EPOLL_SELECTOR_H
#define EPOLL_SELECTOR_H

#include <enums.h>
#include <epoll.h>
#include <Reactor.h>
#include <vector>

class Reactor;

class Selector {
 public:
  virtual void control(int fd,
                         EnumSelectorOption::Option op,
                         uint32_t ev,
                         Reactor *reactor) = 0;
  virtual void select(std::vector<Reactor *> &active_reactors) = 0;
};

#endif //EPOLL_SELECTOR_H
