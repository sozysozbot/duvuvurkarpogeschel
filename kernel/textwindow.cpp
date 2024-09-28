#include "lang/font.hpp"
#include "layer.hpp"
#include "cursored_textbox.hpp"
#include "textwindow.hpp"

void InputTextWindow(PrivilegedCursoredTextBox& box, char32_t unicode) {
  if (unicode == 0) {
    return;
  }

  auto pos = [&box]() { return Vector2D<int>{4 + 8*box.cursor_index, 6}; };

  const int max_chars = (box.text_window->InnerSize().x - 8) / 8 - 1;
  if (unicode == U'\b' && box.cursor_index > 0) {
    box.DrawTextCursor(false);
    --box.cursor_index;
    FillRectangle(*box.text_window->InnerWriter(), pos(), {8, 16}, ToColor(0xffffff));
    box.DrawTextCursor(true);
  } else if (unicode >= ' ' && box.cursor_index < max_chars) {
    box.DrawTextCursor(false);
    // TODO: This should fail once we allow inputting a fullwidth character from the keyboard
    WriteUnicodeChar(*box.text_window->InnerWriter(), pos(), unicode, ToColor(0));
    ++box.cursor_index;
    box.DrawTextCursor(true);
  }

  layer_manager->Draw(box.text_window_layer_id);
}
