#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int pipes1[2], pipes2[2];
  int ret, pid;
  char buf[] = {"a"};
  if (pipe(pipes1) < 0 || pipe(pipes2) < 0) {
    fprintf(2, "pipe error\n");
    exit(1);
  }

  ret = fork();
  if (ret > 0) {
    // father
    pid = getpid();
    // pipes1 : father -> child
    // pipes2 : father <- child
    close(pipes1[0]);
    close(pipes2[1]);
    if (write(pipes1[1], buf, sizeof(buf)) < 0) {
      fprintf(2, "father write error\n");
      exit(1);
    }
    if (read(pipes2[0], buf, sizeof(buf)) < 0) {
      fprintf(2, "father read error\n");
      exit(1);
    }
    printf("%d: received pong\n", pid);
    exit(0);

  } else if (ret == 0) {
    // child
    pid = getpid();
    close(pipes1[1]);
    close(pipes2[0]);
    if (read(pipes1[0], buf, sizeof(buf)) < 0) {
      fprintf(2, "child read error\n");
      exit(1);
    }
    printf("%d: received ping\n", pid);
    if (write(pipes2[1], buf, sizeof(buf)) < 0) {
      fprintf(2, "chilld write error\n");
      exit(1);
    }
    exit(0);
  } else {
    fprintf(2, "fork read error\n");
    exit(1);
  }
}