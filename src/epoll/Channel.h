//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_REACTOR_H
#define EPOLL_REACTOR_H

#include <Processor.h>
#include <enums.h>
#include <io_utility.h>

#include <stdint.h>
#include <functional>
#include <memory>

class Channel;
class Processor;
typedef std::shared_ptr<Channel> ChannelPtr;
typedef std::function<void(ChannelPtr)> ReadCallback;
typedef std::function<void(ChannelPtr)> WriteCallback;
typedef std::function<void(ChannelPtr)> CloseCallback;
typedef std::function<void(ChannelPtr)> ErrorCallback;

class Channel : public std::enable_shared_from_this<Channel> {
 public:
  Channel(int fd, Processor *processor) : fd_(fd), processor_(processor),
                                          reactorStatus_(false) {}

  virtual ~Channel() {}

  virtual void react();

  void enableRead();

  void enableWrite();

  void disableRead();

  void disableWrite();

  void read(IOReader& reader);

  void write(IOWriter& writer);

  void setIncomingEvent(uint32_t event);

  void close();

 protected:
  bool isClosed();

  bool canRead();

  bool canWrite();

  void updateEvents();

  void addEvents();

  void delEvents();

 public://setter & getters
  const ReadCallback &getReadCallback() const;
  void setReadCallback(const ReadCallback &readCallback);

  const WriteCallback &getWriteCallback() const;
  void setWriteCallback(const WriteCallback &writeCallback_);

  const CloseCallback &getCloseCallback() const;
  void setCloseCallback(const CloseCallback &closeCallback_);

  const ErrorCallback &getErrorCallback() const;
  void setErrorCallback(const ErrorCallback &errorCallback_);

  Processor *getProcessor() const;

 private:
  int fd_;
  Processor *processor_;
  EnumSelectorOption::Option op_;
  uint32_t expect_event_;
  uint32_t incoming_event_;
  bool reactorStatus_;

  ReadCallback readCallback_;
  WriteCallback writeCallback_;
  CloseCallback closeCallback_;
  ErrorCallback errorCallback_;
};

#endif //EPOLL_REACTOR_H
