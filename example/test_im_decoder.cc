//
// Created by xiao shutong on 2017/11/23.
//

#include "telnet_im_protocol.h"
#include <string>
#include <iostream>

int main() {
  std::string str = "#Allen# #Lisa# #I Love You#";
  Buffer buf(100);
  buf.write(str.c_str(), str.size());

  TelnetIMDecoder decoder;
  IMUserMsg msg;

  try {
    decoder.unmarshal(buf.getReader(), msg);
    std::cout << msg.from << " speak to " << msg.to << " " << msg.msg <<
                                                                    std::endl;
  } catch (DecodeException& e) {
    std::cout << "exception occurred " << e.what() << std::endl;
  }

  return 0;
}