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

std::shared_ptr<ToplevelWindow> text_window;
unsigned int text_window_layer_id;
void InitializeTextWindow() {
  const int win_w = 168;
  const int win_h = 52;

  text_window = std::make_shared<ToplevelWindow>(
      win_w, win_h, screen_config.pixel_format, "slahurfaesal");
  DrawTextbox(*text_window->InnerWriter(), {0, 0}, text_window->InnerSize());

  text_window_layer_id = layer_manager->NewLayer()
    .SetWindow(text_window)
    .SetDraggable(true)
    .Move({500, 100})
    .ID();

  layer_manager->UpDown(text_window_layer_id, std::numeric_limits<int>::max());
}
int text_window_index;

void DrawTextCursor(bool visible) {
  const auto color = visible ? ToColor(0) : ToColor(0xffffff);
  const auto pos = Vector2D<int>{4 + 8*text_window_index, 5};
  FillRectangle(*text_window->InnerWriter(), pos, {7, 15}, color);
}

void InputTextWindow(char32_t unicode) {
  if (unicode == 0) {
    return;
  }

  auto pos = []() { return Vector2D<int>{4 + 8*text_window_index, 6}; };

  const int max_chars = (text_window->InnerSize().x - 8) / 8 - 1;
  if (unicode == U'\b' && text_window_index > 0) {
    DrawTextCursor(false);
    --text_window_index;
    FillRectangle(*text_window->InnerWriter(), pos(), {8, 16}, ToColor(0xffffff));
    DrawTextCursor(true);
  } else if (unicode >= ' ' && text_window_index < max_chars) {
    DrawTextCursor(false);
    // TODO: This should fail once we allow inputting a fullwidth character from the keyboard
    WriteUnicodeChar(*text_window->InnerWriter(), pos(), unicode, ToColor(0));
    ++text_window_index;
    DrawTextCursor(true);
  }

  layer_manager->Draw(text_window_layer_id);
}
