#pragma once
#include "window.hpp"
struct BuiltInTextBox {
  int cursorTimer;
  unsigned long kTimer;
  bool cursor_visible = false;
  void SetTimer(int timer_id, double timer_sec);
  std::shared_ptr<ToplevelWindow> text_window;
  unsigned int text_window_layer_id;
};
