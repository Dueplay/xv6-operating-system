#include "kernel/types.h"
#include "user/user.h"
// 链式传递，每个进程只打印一次自己的素数
void runprocess(int listenfd) {
  int my_num = 0;
  int forked = 0;
  int pass_num = 0;
  int pipes[2];
  while (1) {
    int read_bytes = read(listenfd, &pass_num, sizeof(pass_num));
    // no data read
    if (read_bytes == 0) {
      close(listenfd);
      if (forked) {
        // tell child no data send
        close(pipes[1]);
        // wait child exit
        int child_pid;
        wait(&child_pid);
      }
      exit(0);
    }
    // process first read
    if (my_num == 0) {
      my_num = pass_num;
      printf("prime %d\n", my_num);
    }
    // process later read , pass along child
    if (pass_num % my_num != 0) {
      if (!forked) {
        pipe(pipes);
        int ret = fork();
        forked = 1;
        if (ret == 0) {
          // child
          close(listenfd);
          close(pipes[1]);
          runprocess(pipes[0]);
        } else {
          // father
          close(pipes[0]);
        }
      }
      // already forked, pass along child by new pipe
      write(pipes[1], &pass_num, sizeof(pass_num));
    }
  }
}

int main(int argc, char *argv[]) {
  int pipes[2];
  pipe(pipes);
  for (int i = 2; i <= 35; i++) {
    write(pipes[1], &i, sizeof(i));
  }
  close(pipes[1]);
  runprocess(pipes[0]);
  exit(0);
}