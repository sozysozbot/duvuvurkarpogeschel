struct BuiltInTextBox {
  int cursorTimer;
  unsigned long kTimer;
  bool cursor_visible = false;
  void SetTimer(int timer_id, double timer_sec);
};
