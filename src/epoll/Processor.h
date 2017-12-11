//
// Created by xiao shutong on 2017/11/16.
//

#ifndef EPOLL_PROSESSOR_H
#define EPOLL_PROSESSOR_H

#include <Selector.h>
#include <Channel.h>
#include <vector>
#include <memory>
#include <map>

class Selector;
class Channel;

typedef std::unique_ptr<Selector> SelectorUP;
typedef std::shared_ptr<Channel> ChannelSP;
typedef std::map<uintptr_t, ChannelSP> PointerChannelMap;

class Processor {
 public:
  explicit Processor(SelectorUP &&selector);

  void update(int fd,
              EnumSelectorOption::Option op,
              uint32_t ev,
              ChannelSP reactor);

  void process();

 private:
  SelectorUP selector_;
  PointerChannelMap channels_;
};

typedef std::shared_ptr<Processor> ProcessorSP;
typedef std::weak_ptr<Processor> ProcessorWP;
#endif //EPOLL_PROSESSOR_H
