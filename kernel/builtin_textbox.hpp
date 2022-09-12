struct BuiltInTextBox {
  const int cursorTimer;
  const unsigned long kTimer;
  bool cursor_visible = false;
  BuiltInTextBox(int timer_id, double timer_sec);
};
