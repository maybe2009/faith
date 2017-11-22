//
// Created by DW on 2017/11/16.
//

#include "Reactor.h"

void Reactor::updateEvents() {
  if (!reactorStatus_) {
    op_ = EnumSelectorOption::Option::ADD;
    reactorStatus_ = true;
  } else {
    op_ = EnumSelectorOption::Option::MOD;
  }
  processor_->update(fd_, op_, expect_event_, this);
}

void Reactor::wannaRead() {
  expect_event_ |= EnumSelectorEvent::Event::ON_READ;
  updateEvents();
}

void Reactor::wannaWrite() {
  expect_event_ |= EnumSelectorEvent::Event::ON_WRITE;
  updateEvents();
}

void Reactor::addEvents() {
  op_ = EnumSelectorOption::Option::ADD;
  processor_->update(fd_, EnumSelectorOption::Option::ADD, expect_event_, this);
}

void Reactor::delEvents() {
  op_ = EnumSelectorOption::Option::ADD;
  processor_->update(fd_, EnumSelectorOption::Option::DEL, expect_event_, this);
}

bool Reactor::canRead() {
  return (incoming_event_ & EnumSelectorEvent::Event::ON_READ)!=0;
}

bool Reactor::canWrite() {
  return (incoming_event_& EnumSelectorEvent::Event::ON_WRITE)!=0;
}

bool Reactor::isClosed() {
  return (incoming_event_ & EnumSelectorEvent::Event::ON_CLOSE)!=0;
}

void Reactor::setIncomingEvent(uint32_t event) {
  incoming_event_ = event;
}

void Reactor::disableRead() {
  expect_event_ ^= EnumSelectorEvent::Event::ON_READ;
  updateEvents();
}

void Reactor::disableWrite() {
  expect_event_ ^= EnumSelectorEvent::Event::ON_WRITE;
  updateEvents();
}

