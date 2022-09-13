#include <vector>
#include "font.hpp"
#include "layer.hpp"
#include "cursored_textbox.hpp"
#include "textwindowpekzep.hpp"
#include "praige_r_dict.h"

struct IMEState {
  std::vector<char32_t> solidified;
  int cursor_index;
  std::vector<char32_t> non_solidified;
  std::vector<std::string> candidates;
  int candidate_index;
};

void InputTextWindowPekzep(CursoredTextBox& box, char32_t unicode, uint8_t modifier) {
  static IMEState state = {};
  if (unicode == 0) {
    return;
  }

  auto cursor_pos = [&box]() { return Vector2D<int>{4 + 8*box.cursor_index, 6}; };

  const int max_chars = (box.text_window->InnerSize().x - 8) / 8 - 1;
  if (unicode == U'\b' && box.cursor_index > 0) {
    box.DrawTextCursor(false);
    --box.cursor_index;
    FillRectangle(*box.text_window->InnerWriter(), cursor_pos(), {8, 16}, ToColor(0xffffff));
    box.DrawTextCursor(true);
  } else if (unicode >= ' ' && box.cursor_index < max_chars) {
    box.DrawTextCursor(false);
    // TODO: This should fail once we allow inputting a fullwidth character from the keyboard
    WriteUnicodeChar(*box.text_window->InnerWriter(), cursor_pos(), unicode, ToColor(0));
    ++box.cursor_index;
    box.DrawTextCursor(true);
  }

  layer_manager->Draw(box.text_window_layer_id);
}
