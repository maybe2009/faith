//
// Created by xiao shutong on 2018/3/12.
//

#include <net.h>
#include <iostream>
#include <signal.h>

using namespace std;
using namespace net;

void signalPipe(int signal) {
  cout << "signal pipe occured: " << signal << endl;
}

int main(int argc, const char *argv[]) {
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }

  signal(SIGPIPE, signalPipe);

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  unique_ptr<TcpListener> listen(TcpListen(ip.c_str(), port));
  while(1) {
    TcpConn *conn = listen->Accept();

    char buf[1024];
    char *ptr = buf;
    uint32_t count = 20;

    ssize_t ret = 0;
    do {
      ret = conn->reader.readn(ptr++, 1);
      //cout << "read reurn " << ret << endl;
    } while (ret > 0 && count-- > 0);
    *ptr = 0;
    cout << "client request: " << buf << endl;

    cout << "first write return "
         << conn->writer.writen("Hello human.", sizeof("Hello human.")) << endl;

    sleep(5);

    if (ret==0) {
      ret = conn->reader.readn(ptr++, 1);
      cout << "read more ret " << ret << endl;

      cout << "second write return "
           << conn->writer.writen("Hello!", sizeof("Hello!")) << endl;
      sleep(10);

      cout << "third write return "
           << conn->writer.writen("Hello!", sizeof("Hello!")) << endl;

      conn->close();
    }
  }

  cout << "bye" << endl;

  sleep(1);
  return 0;
}