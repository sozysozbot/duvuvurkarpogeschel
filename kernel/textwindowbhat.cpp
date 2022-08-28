#include <cstdint>
#include <cstddef>
#include <limits>
#include <numeric>

#include "font.hpp"
#include "memory_manager.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "terminal.hpp"
#include "textwindowbhat.hpp"
#include "syscall.hpp"

std::shared_ptr<ToplevelWindow> text_window_bhat;
unsigned int text_window_bhat_layer_id;
void InitializeTextWindowBhat() {
  const int win_w = 368;
  const int win_h = 52;

  text_window_bhat = std::make_shared<ToplevelWindow>(
      win_w, win_h, screen_config.pixel_format, "phertarsvirle'i slahurfaesal");
  DrawTextbox(*text_window_bhat->InnerWriter(), {0, 0}, text_window_bhat->InnerSize());

  text_window_bhat_layer_id = layer_manager->NewLayer()
    .SetWindow(text_window_bhat)
    .SetDraggable(true)
    .Move({300, 45})
    .ID();

  layer_manager->UpDown(text_window_bhat_layer_id, std::numeric_limits<int>::max());
}
int text_window_bhat_index;

void DrawTextCursorBhat(bool visible) {
  const auto color = visible ? ToColor(0) : ToColor(0xffffff);
  const auto pos = Vector2D<int>{4 + 8*text_window_bhat_index, 5};
  FillRectangle(*text_window_bhat->InnerWriter(), pos, {7, 15}, color);
}

void InputTextWindowBhat(char32_t unicode) {
  if (unicode == 0) {
    return;
  }

  auto pos = []() { return Vector2D<int>{4 + 8*text_window_bhat_index, 6}; };

  const int max_chars = (text_window_bhat->InnerSize().x - 8) / 8 - 1;
  if (unicode == U'\b' && text_window_bhat_index > 0) {
    DrawTextCursorBhat(false);
    --text_window_bhat_index;
    FillRectangle(*text_window_bhat->InnerWriter(), pos(), {8, 16}, ToColor(0xffffff));
    DrawTextCursorBhat(true);
  } else if (unicode >= ' ' && text_window_bhat_index < max_chars) {
    DrawTextCursorBhat(false);
    // TODO: This should fail once we allow inputting a fullwidth character from the keyboard
    WriteUnicodeChar(*text_window_bhat->InnerWriter(), pos(), unicode, ToColor(0));
    ++text_window_bhat_index;
    DrawTextCursorBhat(true);
  }

  layer_manager->Draw(text_window_bhat_layer_id);
}
