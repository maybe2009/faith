//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_SELECTOR_H
#define EPOLL_SELECTOR_H

#include <enums.h>
#include <epoll.h>
#include <Channel.h>
#include <vector>
#include <memory>

class Channel;

class Selector {
 public:
  virtual  ~Selector() {};

  virtual void control(int fd,
                         EnumSelectorOption::Option op,
                         uint32_t ev,
                         Channel *reactor) = 0;
  virtual void select(std::vector<Channel *> &active_reactors) = 0;
};

typedef std::unique_ptr<Selector> SelectorUP;

#endif //EPOLL_SELECTOR_H
