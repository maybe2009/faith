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

  unique_ptr<TcpListener> listen(TcpListen(ip.c_str(), port));

  uint32_t i = 20;
  do {
    TcpConn* conn = listen->Accept();

    char buf[1024];
    char* ptr = buf;
    uint32_t count = 20;

    ssize_t  ret = 0;
    do {
      ret = conn->reader.readn(ptr++, 1);
      cout << "read reurn " << ret << endl;
    } while(ret > 0 && count-- > 0);
    *ptr = 0;
    cout << "client request: " << buf << endl;

    if (conn->writer.writen("Hello human.", sizeof("Hello human.")) < 0) {
      cout << "write error" << endl;
    }

    if (ret == 0) {
      ret = conn->reader.readn(ptr++, 1);
      cout << "sss ret " << ret << endl;
      conn->close();
    }
  } while(i-- > 0);

  cout << "bye" << endl;
  return 0;
}