#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

// find file name.ie find last / pos
char *basename(char *pathname) {
  char *prev = 0;
  char *curr = strchr(pathname, '/');
  while (curr != 0) {
    prev = curr;
    curr = strchr(curr + 1, '/');
  }
  return prev;
}

void find(char *cur_path, char *target) {
  int fd;
  char buf[512], *p, *file_name;
  struct stat st;
  struct dirent de;
  if ((fd = open(cur_path, O_RDONLY)) < 0) {
    fprintf(2, "cannot open %s\n", cur_path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "cannot stat %s\n", cur_path);
    close(fd);
    return;
  }
  switch (st.type) {
    // file case,match cur and target
  case T_FILE:
    file_name = basename(cur_path);
    int match = 1;
    if (file_name == 0 || strcmp(file_name + 1, target) != 0) {
      match = 0;
    }
    if (match) {
      printf("%s\n", cur_path);
    }
    close(fd);
    break;

  case T_DIR:
    // dir,for_each dirent to recusive find
    memset(buf, 0, sizeof(buf));
    uint cur_path_len = strlen(cur_path);
    memcpy(buf, cur_path, cur_path_len);
    buf[cur_path_len] = '/';
    p = buf + cur_path_len + 1;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0) {
        continue;
      }
      memcpy(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, target);
    }
    close(fd);
    break;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "usage: find [dir] [target file]\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}