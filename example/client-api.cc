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
  char buf[22];
  char* ptr = buf;
  do {
    char c;
    ret = conn->reader.readn(&c, 1);
    if (ret <= 0) {
      cout << "no, it can't be!" << endl;
    }
    *ptr++ = c;
  } while(n-- > 0 && ret > 0);
  *ptr = 0;
  cout << "read after shutdown write " << buf << endl;

  //But wo can't write, if we do so, SIGPIPE well be triggered at once
  signal(SIGPIPE, signalPipe);
  ret = conn->writer.writen("Hello!", strlen("Hello!"));
  cout << "write after shutdown write return " << ret << endl;

  return;
}

void TestShutdownReadClient(std::string ip, uint16_t port) {
  unique_ptr<TcpConn> conn(TcpDial(ip.c_str(), port));

  //close it make our test easy :-)
  conn->tcpNoDelay();

  //Ok, shut down read, no tcp state change is gonna happen
  conn->shutdownRead();

  //We can still write
  int ret = 0;
  std::string safeWord = "I have shutdown my read";
  ret = conn->writer.writen(safeWord.c_str(), safeWord.size());
  if (ret != safeWord.size()) {
    cout << "no, it can't be!" << endl;
  }
  cout << "write after shutdown write return " << ret << endl;

  //Can we read? It's platform depended. Books or manuals will tell you
  //that "you can't, any further reading will return 0 which means EOF",
  // but it depends actually.
  // On my ubuntu 16.04 virtual machine any further read is ok as usual
  int n = 3;
  char buf[5];
  char* ptr = buf;
  do {
    char c;
    ret = conn->reader.readn(&c, 1);
    if (ret < 0) {
      cout << "no, it can't be!" << endl;
    } else if (ret == 0) {
      // Be careful, we may read server EOF because we have shutdown the
      // read. But, as well as the peer start writing data, we can read.
      cout << "oh, EOF detected, read after shutdown read return "
          << ret << endl;
    } else {
      *ptr++ = c;
     // cout << "got " << c << endl;
      if (n-- <= 0) {
        break;
      }
    }
  } while(ret >= 0);

  *ptr = 0;
  cout << " read after shutdown read " << buf << endl;

  return;
}

int main(int argc, const char *argv[]) {
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  TestShutdownReadClient(ip, port);

  cout << "bye" << endl;
  return 0;
}