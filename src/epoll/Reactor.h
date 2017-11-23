//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_REACTOR_H
#define EPOLL_REACTOR_H

#include <Processor.h>
#include <enums.h>

#include <stdint.h>

class Processor;

class Reactor {
 public:
  Reactor(int fd, Processor* processor) : fd_(fd), processor_(processor),
                                          reactorStatus_(false) {}

  virtual ~Reactor() {}

  virtual void react() = 0;

  void wannaRead();

  void wannaWrite();

  void disableRead();

  void disableWrite();

  void setIncomingEvent(uint32_t event);

 protected:
  bool isClosed();

  bool canRead();

  bool canWrite();

  void updateEvents();

  void addEvents();

  void delEvents();

  Processor* getProcessor() const {
    return processor_;
  }
 private:
  int fd_;
  Processor* processor_;
  EnumSelectorOption::Option op_;
  uint32_t expect_event_;
  uint32_t incoming_event_;
  bool reactorStatus_;
};

#endif //EPOLL_REACTOR_H
