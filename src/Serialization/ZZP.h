//
// Created by xiao shutong on 2017/12/18.
//

#ifndef EPOLL_ZZP_H
#define EPOLL_ZZP_H

#include <Buffer.h>
#include <stdint.h>
#include <vector>
#include <list>
#include <deque>
#include <iostream>
#include <typeinfo>

template<typename T>
Buffer &operator<<(Buffer &buffer, const T &data) {
  buffer.push(&data);
}

template<typename T>
Buffer &push_sequence_container(Buffer &buffer, const T &data) {
  for (const auto& d : data) {
    std::cout << "type " << typeid(d).name() << std::endl;
    buffer.push(&d);

  }
}

template<typename T>
Buffer& operator<<(Buffer& buffer, const std::vector<T>& vec) {
  push_sequence_container(buffer, vec);
}

template<typename T>
Buffer& operator<<(Buffer& buffer, const std::list<T>& list) {
  push_sequence_container(buffer, list);
}

template<typename T>
Buffer& operator<<(Buffer& buffer, const std::deque<T>& deque) {
  push_sequence_container(buffer, deque);
}

#endif //EPOLL_ZZP_H
