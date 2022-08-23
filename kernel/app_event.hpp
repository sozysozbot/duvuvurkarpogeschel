#pragma once

#ifdef __cplusplus
extern "C" {
#else
#include <stddef.h>
#include <stdint.h>

// #include <uchar.h>
// uchar.h somehow did not exist in this environment.
#define char32_t uint_least32_t
#endif

struct AppEvent {
  enum Type {
    kQuit,
    kMouseMove,
    kMouseButton,
    kTimerTimeout,
    kKeyPush,
  } type;

  union {
    struct {
      int x, y;
      int dx, dy;
      uint8_t buttons;
    } mouse_move;

    struct {
      int x, y;
      int press; // 1: press, 0: release
      int button;
    } mouse_button;

    struct {
      unsigned long timeout;
      int value;
    } timer;

    struct {
      uint8_t modifier;
      uint8_t keycode;
      char32_t unicode;
      int press; // 1: press, 0: release
    } keypush;
  } arg;
};

#ifdef __cplusplus
} // extern "C"
#endif
