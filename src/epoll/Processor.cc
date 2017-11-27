//
// Created by xiao shutong on 2017/11/16.
//

#include "Processor.h"

#include <iostream>
#include <assert.h>

Processor::Processor(Selector *selector) : selector_(selector) {

}

void Processor::update(int fd,
                       EnumSelectorOption::Option op,
                       uint32_t ev,
                       Channel *reactor) {
  selector_->control(fd, op, ev, reactor);
}

void Processor::process() {
  std::vector<Channel *> active_reactors;
  selector_->select(active_reactors);

  for (auto active : active_reactors) {
    assert(active != nullptr);
    std::cout << active << std::endl;
    active->react();
  }
}
