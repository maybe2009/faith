//
// Created by xiao shutong on 2017/11/16.
//

#include "Processor.h"

#include <iostream>
#include <assert.h>

Processor::Processor(SelectorUP &&selector) : selector_(std::move(selector)) {}

void Processor::update(int fd,
                       EnumSelectorOption::Option op,
                       uint32_t ev,
                       ChannelSP reactor) {
  if (EnumSelectorOption::Option::MOD==op ||
      EnumSelectorOption::Option::ADD==op) {
    channels_.insert(make_pair(reinterpret_cast<uintptr_t>(reactor.get()),
                               reactor));
  } else if (EnumSelectorOption::Option::DEL==op) {
    channels_.erase(reinterpret_cast<uintptr_t >(reactor.get()));
  }

  selector_->control(fd, op, ev, reactor.get());
}

void Processor::process() {
  std::vector<Channel *> active_reactors;
  selector_->select(active_reactors);

  for (auto active : active_reactors) {
    assert(active!=nullptr);

    ChannelSP &channel = channels_.at(reinterpret_cast<uintptr_t >(active));
    std::cout << channel.get() << std::endl;
    channel->react();
  }
}
