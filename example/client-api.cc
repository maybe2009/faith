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

void TestShutdownWriteClient(std::string ip, uint16_t port) {
  unique_ptr<TcpConn> conn(TcpDial(ip.c_str(), port));

  //close it make our test easy :-)
  conn->tcpNoDelay();

  //Ok, shut down write, FIN is sent
  conn->shutdownWrite();

  //We are in FIN_WAIT2 state now

  //We can read as usual
  uint32_t n = 20;
  int ret = 0;
  do {
    char c;
    ret = conn->reader.readn(&c, 1);
    if (ret < 0) {
      cout << "no, it can't be!" << endl;
    }
  } while(n-- > 0 && ret > 0);

  //But wo can't write, if we do so, SIGPIPE well be triggered at once
  signal(SIGPIPE, signalPipe);
  ret = conn->writer.writen("Hello!", strlen("Hello!"));
  cout << "write after shutdown write return " << ret << endl;

  return;
}

int main(int argc, const char *argv[]) {
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  TestShutdownWriteClient(ip, port);

  cout << "bye" << endl;
  return 0;
}