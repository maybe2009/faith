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

void TestShutdownWriteServer(std::string ip, uint16_t port) {
  unique_ptr<TcpListener> listener(TcpListen(ip.c_str(), port));
  unique_ptr<TcpConn> conn(listener->Accept());

  //Read until EOF
  int ret = 0;
  do {
    char c;
    ret = conn->reader.readn(&c, 1);
  } while (ret > 0);

  //Ok, client has sent FIN and step into FIN_WAIT2 now, we are in CLOSE_WAIT

  //We can still write as usual
  int count = 20;
  do {
    ret = conn->writer.writen("a", 1);
    if (ret <= 0) {
      cout << "no, it can't be!" << endl;
    }
  } while(count-- > 0 && ret > 0);

  //If we read again, EOF is returned
  char c;
  ret = conn->reader.readn(&c, 1);
  if (ret != 0) {
    cout << "no, it can't be!" << endl;
  }

  return;
}

int main(int argc, const char *argv[]) {
  if (argc!=3) {
    cout << "argument: ip port" << endl;
    exit(1);
  }

  string ip(argv[1]);
  uint16_t port = atoi(argv[2]);

  TestShutdownWriteServer(ip, port);

  cout << "bye" << endl;
  return 0;
}