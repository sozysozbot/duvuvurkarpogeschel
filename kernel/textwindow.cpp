#include <cstdint>
#include <cstddef>
#include <limits>
#include <numeric>

#include "font.hpp"
#include "memory_manager.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "terminal.hpp"
#include "textwindow.hpp"
#include "syscall.hpp"
#include "builtin_textbox.hpp"

void InitializeTextWindow(BuiltInTextBox& box, int win_w, int win_h, const char *title, Vector2D<int> pos) {
  box.text_window = std::make_shared<ToplevelWindow>(
      win_w, win_h, screen_config.pixel_format, title);
  DrawTextbox(*box.text_window->InnerWriter(), {0, 0}, box.text_window->InnerSize());

  box.text_window_layer_id = layer_manager->NewLayer()
    .SetWindow(box.text_window)
    .SetDraggable(true)
    .Move(pos)
    .ID();

  layer_manager->UpDown(box.text_window_layer_id, std::numeric_limits<int>::max());
}

void DrawTextCursor(BuiltInTextBox& box, bool visible) {
  const auto color = visible ? ToColor(0) : ToColor(0xffffff);
  const auto pos = Vector2D<int>{4 + 8*box.text_window_index, 5};
  FillRectangle(*box.text_window->InnerWriter(), pos, {7, 15}, color);
}

void InputTextWindow(BuiltInTextBox& box, char32_t unicode) {
  if (unicode == 0) {
    return;
  }

  auto pos = [&box]() { return Vector2D<int>{4 + 8*box.text_window_index, 6}; };

  const int max_chars = (box.text_window->InnerSize().x - 8) / 8 - 1;
  if (unicode == U'\b' && box.text_window_index > 0) {
    DrawTextCursor(box, false);
    --box.text_window_index;
    FillRectangle(*box.text_window->InnerWriter(), pos(), {8, 16}, ToColor(0xffffff));
    DrawTextCursor(box, true);
  } else if (unicode >= ' ' && box.text_window_index < max_chars) {
    DrawTextCursor(box, false);
    // TODO: This should fail once we allow inputting a fullwidth character from the keyboard
    WriteUnicodeChar(*box.text_window->InnerWriter(), pos(), unicode, ToColor(0));
    ++box.text_window_index;
    DrawTextCursor(box, true);
  }

  layer_manager->Draw(box.text_window_layer_id);
}
