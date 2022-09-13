#pragma once
#include "window.hpp"
struct CursoredTextBox {
  unsigned long kTimer;
  bool cursor_visible = false;
  std::shared_ptr<ToplevelWindow> text_window;
  int cursor_index;
  int cursorTimer;
  unsigned int text_window_layer_id;
  void SetTimer(int timer_id, double timer_sec);
  void InitializeTextWindow(int win_w, int win_h, const char *title, Vector2D<int> pos, int bottom_additional_margin = 0);
  void DrawTextCursor(bool visible);
  void ManageCursor(std::optional<Message> msg);
};
