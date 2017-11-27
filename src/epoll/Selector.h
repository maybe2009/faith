//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_SELECTOR_H
#define EPOLL_SELECTOR_H

#include <enums.h>
#include <epoll.h>
#include <Channel.h>
#include <vector>

class Channel;

class Selector {
 public:
  virtual void control(int fd,
                         EnumSelectorOption::Option op,
                         uint32_t ev,
                         Channel *reactor) = 0;
  virtual void select(std::vector<Channel *> &active_reactors) = 0;
};

#endif //EPOLL_SELECTOR_H
