#pragma once
#include "window.hpp"
struct BuiltInTextBox {
  int cursorTimer;
  unsigned long kTimer;
  bool cursor_visible = false;
  std::shared_ptr<ToplevelWindow> text_window;
  unsigned int text_window_layer_id;
  int text_window_index;
  void SetTimer(int timer_id, double timer_sec);
  void InitializeTextWindow(int win_w, int win_h, const char *title, Vector2D<int> pos);
};
