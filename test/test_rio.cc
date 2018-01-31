//
// Created by xiao shutong on 2018/1/31.
//
#include <stdio.h>
#include <fcntl.h>
#include <rio.h>

int main(int argc,  const char* argv[]) {
  if (argc < 2) {
    printf("argc < 2\n");
  }

  int fd = open(argv[1], O_CREAT | O_RDWR, 0666);
  if (fd < 0) {
    printf("open file failed: %s\n, %d", argv[2], errno);
    return -1;
  }

//  for (auto i = 0; i < 10; i++) {
//    char c;
//    rio_readn(fd, &c, 1);
//    printf("%c", c);
//  }
//  printf("\n");

  rio_t reader;
  rio_init(&reader, fd);
  char buf[100];
  size_t line = 0;
  ssize_t rc = 0;

  while ((rc = rio_breadline(&reader, buf, 99)) > 0) {
    printf("%s\n", buf);
  }

  return 0;
}
