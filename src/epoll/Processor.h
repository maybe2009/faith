//
// Created by DW on 2017/11/16.
//

#ifndef EPOLL_PROSESSOR_H
#define EPOLL_PROSESSOR_H

#include <Selector.h>
#include <Reactor.h>
#include <vector>

class Selector;
class Reactor;

class Processor {
 public:
  explicit Processor(Selector *selector);

  void update(int fd,
                EnumSelectorOption::Option op,
                uint32_t ev,
                Reactor *reactor);

  void process();

 private:
  Selector *selector_;
};

#endif //EPOLL_PROSESSOR_H
