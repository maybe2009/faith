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

  //signal(SIGPIPE, signalPipe);

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  unique_ptr<TcpConn> conn(TcpDial(ip.c_str(), port));

  //conn->tcpNoDelay();
  conn->shutdownRead();

  char buf[1024];
  char* ptr = buf;
  uint32_t count = 100;
  ssize_t ret = 0;
  do {
    ret = conn->reader.readn(ptr, 1);
    cout << "client read ret " << ret << " p " << *ptr++ << endl;
    sleep(1);
  } while (count-- > 0);
  *ptr = 0;
  cout << "server reply: " << buf << endl;

  //conn->close();

  cout << "bye" << endl;

  return 0;
}