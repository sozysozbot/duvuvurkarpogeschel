#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../syscall.h"

extern "C" void main(int argc, char** argv) {
  auto [layer_id, err_openwin]
    = SyscallOpenWindow(400, 100, 10, 10, u8"皇論之書");
  if (err_openwin) {
    exit(err_openwin);
  }

  SyscallWinWriteString(layer_id, 7, 24, 0xc00000, u8"地開混在 天光心在 心集皇来 皇生心集");
  SyscallWinWriteString(layer_id, 24, 40, 0x00c000, u8"人心其片 心開皇行 心圧行無 唯心唯思");
  SyscallWinWriteString(layer_id, 40, 56, 0x0000c0, u8"心圧圧来 圧来圧来 心圧悪来 心圧唯圧");

  AppEvent events[1];
  while (true) {
    auto [ n, err ] = SyscallReadEvent(events, 1);
    if (err) {
      printf("mi nix mels xelo voleso. %s\n", strerror(err));
      break;
    }
    if (events[0].type == AppEvent::kQuit) {
      break;
    } else if (events[0].type == AppEvent::kMouseMove ||
        events[0].type == AppEvent::kMouseButton ||
        events[0].type == AppEvent::kKeyPush) {
      // ignore
    } else {
      printf("qunenerfe volesosti. akrapt es %d\n", events[0].type);
    }
  }
  SyscallCloseWindow(layer_id);
  exit(0);
}
