//
// Created by xiao shutong on 2017/12/18.
//

#include <ZZP.h>
#include <vector>

using namespace std;

int main() {
  vector<int> vec {1, 2, 3, 4};
  Buffer buf(100);
  buf << vec;
  buf.print();

  vector<char> vec2{'a', 'b', 'c', 'd'};
  buf.clear();
  buf << vec2;
  buf.print();
  return 0;
}