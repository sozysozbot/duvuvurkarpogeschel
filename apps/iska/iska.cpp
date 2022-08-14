#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../syscall.h"

extern "C" void main(int argc, char** argv) {
  const char* cmd = "delok";
  if (argc >= 2) {
    cmd = argv[1];
  }

  if (strcmp(cmd, "delok") == 0) {
    __asm__("hlt");
  } else if (strcmp(cmd, "kr_sysital") == 0) {
    int* p = reinterpret_cast<int*>(0x100);
    *p = 42;
  } else if (strcmp(cmd, "kr_karfaksal") == 0) {
    int* p = reinterpret_cast<int*>(0xffff8000ffff0000);
    *p = 123;
  } else if (strcmp(cmd, "kakites") == 0) {
    volatile int z = 0;
    printf("100/%d = %d\n", z, 100/z);
  }

  exit(0);
}
