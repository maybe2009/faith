//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_PROSESSOR_H
#define EPOLL_PROSESSOR_H

#include <Selector.h>
#include <Channel.h>
#include <vector>

class Selector;
class Channel;

class Processor {
 public:
  explicit Processor(Selector *selector);

  void update(int fd,
                EnumSelectorOption::Option op,
                uint32_t ev,
                Channel *reactor);

  void process();

 private:
  Selector *selector_;
};

#endif //EPOLL_PROSESSOR_H
