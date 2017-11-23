//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_EPOLLSELECTOR_H
#define EPOLL_EPOLLSELECTOR_H

#include <Reactor.h>
#include "Selector.h"

#include <iostream>
#include <assert.h>
#include <stdio.h>

const uint32_t MAX_EVENTS = 10;

class EpollSelector : public Selector {
 public:
  EpollSelector() : epoll_(nullptr) {
    epoll_ = make_epoll();
  }

  virtual void control(int fd,
                         EnumSelectorOption::Option op,
                         uint32_t ev,
                         Reactor *reactor) override {
    struct epoll_event event;
    event.data.ptr = reactor;

    event.events = 0;
    if (EnumSelectorEvent::Event::ON_READ & ev) {
      event.events |= (EPOLLIN | EPOLLRDHUP);
    }

    if (EnumSelectorEvent::Event::ON_WRITE & ev) {
      event.events |= (EPOLLOUT | EPOLLRDHUP);
    }

    int option = 0;
    if (EnumSelectorOption::Option::ADD==op) {
      option = EPOLL_CTL_ADD;
    } else if (EnumSelectorOption::Option::DEL == op) {
      option = EPOLL_CTL_DEL;
    } else if (EnumSelectorOption::Option::MOD == op) {
      option = EPOLL_CTL_MOD;
    }

    printf("option %u, fd %d events %x\n", option, fd, event.events);
    epoll_->control(option, fd, &event);
  }

  virtual void select(std::vector<Reactor *> &active_reactors) {
    struct epoll_event active_events[MAX_EVENTS];

    std::cout << "epoll select" << std::endl;
    int events_num = epoll_->select((struct epoll_event *) active_events,
                                    MAX_EVENTS, -1);
    if (events_num==-1) {
      std::cout << "error" << std::endl;
    }
    std::cout << "events number " << events_num << std::endl;

    for (uint32_t i = 0; i < events_num; i++) {
      struct epoll_event *event = active_events + i;
      Reactor *reactor = static_cast<Reactor *>(event->data.ptr);
      assert(reactor!=nullptr);

      printf("incoming event is %x\n", event->events);
      int incoming_event = 0;

      if (event->events & EPOLLRDHUP) {
        incoming_event |= EnumSelectorEvent::Event::ON_CLOSE;
      }

      if (event->events & EPOLLIN) {
        incoming_event |= EnumSelectorEvent::Event::ON_READ;
      }

      if (event->events & EPOLLOUT) {
        incoming_event |= EnumSelectorEvent::Event ::ON_WRITE;
      }

      std::cout << "incoming event is " << event->events << std::endl;
      reactor->setIncomingEvent(incoming_event);

      active_reactors.push_back(reactor);
    }
  }

 private:
  Epoll *epoll_;
};

#endif //EPOLL_EPOLLSELECTOR_H
