//
// Created by xiao shutong on 2018/3/12.
//

#include <net.h>
#include <iostream>

using namespace std;
using namespace net;

int main(int argc, const char *argv[]) {
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  unique_ptr<TcpConn> conn(TcpDial(ip.c_str(), port));

  cout << "write" << endl;
  conn->writer.writen("Hello, World!", sizeof("Hello, World!"));
  cout << "write complete" << endl;
  conn->shutdownWrite();

  char buf[1024];
  char* ptr = buf;
  uint32_t count = 20;
  while (conn->reader.readn(ptr++, 1) > 0 && count-- > 0);
  *ptr = 0;
  cout << "server reply: " << buf << endl;

  conn->close();

  cout << "bye" << endl;
  return 0;
}