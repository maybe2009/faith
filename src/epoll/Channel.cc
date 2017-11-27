//
// Created by xiao shutong on 2017/11/16.
//

#include "Channel.h"
#include <iostream>
void Channel::updateEvents() {
  if (!reactorStatus_) {
    op_ = EnumSelectorOption::Option::ADD;
    reactorStatus_ = true;
  } else {
    op_ = EnumSelectorOption::Option::MOD;
  }
  processor_->update(fd_, op_, expect_event_, this);
}

void Channel::enableRead() {
  expect_event_ |= EnumSelectorEvent::Event::ON_READ;
  updateEvents();
}

void Channel::enableWrite() {
  expect_event_ |= EnumSelectorEvent::Event::ON_WRITE;
  updateEvents();
}

void Channel::addEvents() {
  op_ = EnumSelectorOption::Option::ADD;
  processor_->update(fd_, EnumSelectorOption::Option::ADD, expect_event_, this);
}

void Channel::delEvents() {
  op_ = EnumSelectorOption::Option::ADD;
  processor_->update(fd_, EnumSelectorOption::Option::DEL, expect_event_, this);
}

bool Channel::canRead() {
  return (incoming_event_ & EnumSelectorEvent::Event::ON_READ)!=0;
}

bool Channel::canWrite() {
  return (incoming_event_& EnumSelectorEvent::Event::ON_WRITE)!=0;
}

bool Channel::isClosed() {
  return (incoming_event_ & EnumSelectorEvent::Event::ON_CLOSE)!=0;
}

void Channel::setIncomingEvent(uint32_t event) {
  incoming_event_ = event;
}

void Channel::disableRead() {
  expect_event_ ^= EnumSelectorEvent::Event::ON_READ;
  updateEvents();
}

void Channel::disableWrite() {
  expect_event_ ^= EnumSelectorEvent::Event::ON_WRITE;
  updateEvents();
}

void Channel::react() {
  if (isClosed()) {
    closeCallback_(shared_from_this());
    return;
  }

  if (canRead()) {
    std::cout << "on read" << std::endl;
    readCallback_(shared_from_this());
  }

  if (canWrite()) {
    writeCallback_(shared_from_this());
  }
}

const ReadCallback &Channel::getReadCallback() const {
  return readCallback_;
}
void Channel::setReadCallback(const ReadCallback &readCallback) {
  Channel::readCallback_ = readCallback;
}
const WriteCallback &Channel::getWriteCallback() const {
  return writeCallback_;
}

void Channel::setWriteCallback(const WriteCallback &writeCallback_) {
  Channel::writeCallback_ = writeCallback_;
}

const CloseCallback &Channel::getCloseCallback() const {
  return closeCallback_;
}

void Channel::setCloseCallback(const CloseCallback &closeCallback_) {
  Channel::closeCallback_ = closeCallback_;
}

const ErrorCallback &Channel::getErrorCallback() const {
  return errorCallback_;
}

void Channel::setErrorCallback(const ErrorCallback &errorCallback_) {
  Channel::errorCallback_ = errorCallback_;
}

Processor *Channel::getProcessor() const {
  return processor_;
}

void Channel::read(IOReader &reader) {
  reader.read(fd_);
}

void Channel::write(IOWriter &writer) {
  writer.write(fd_);
}

void Channel::close() {
  ::close(fd_);
}


