#include <vector>
#include "font.hpp"
#include "layer.hpp"
#include "cursored_textbox.hpp"
#include "textwindowpekzep.hpp"
#include "praige_r_dict.hpp"

struct IMEState {
  std::vector<char32_t> solidified;
  std::vector<char32_t> non_solidified;
  int candidate_index;
  std::vector<std::u32string> candidates;
  void Render(CursoredTextBox& box);
  void ComputeCandidatesAndStore();
};

bool startsWith(const char32_t* haystack, std::vector<char32_t>& needle) {
  for (int i = 0; i < needle.size() ; i++) {
    if (haystack[i] == '\0') { return false; }
    if (haystack[i] != needle[i]) { return false; }
  }
  return true;
}

void IMEState::ComputeCandidatesAndStore() {
  this->candidates.clear();

  if (this->non_solidified.empty()) return;
  for (PekzepChar *c = dict; c != dict_end; c++) {
    if (startsWith(c->praige, this->non_solidified)) {
      this->candidates.push_back(c->hanzi);
    }
  }
}

void IMEState::Render(CursoredTextBox& box) {
  // first, erase everything
  box.ClearTextWindow();

  // then render
  box.DrawTextCursor(false);
  int solidified_width = WriteUTF32CharVec(*box.text_window->InnerWriter(), Vector2D<int>{4, 6}, this->solidified, ToColor(0));
  int unsolidified_width = WriteUTF32CharVec(
    *box.text_window->InnerWriter(), 
    Vector2D<int>{4 + 8 * solidified_width, 6}, 
    this->non_solidified, ToColor(0xe916c3)
  );
  box.cursor_index = solidified_width + unsolidified_width;
  box.DrawTextCursor(true);

  ComputeCandidatesAndStore();
  // want to prepare a string of the form U"<此>時 火 車 善 子"
  std::u32string candidate_display = U"";
  for (int i = 0; i < this->candidates.size(); i++) {
    candidate_display += i == this->candidate_index ? U"<" : i == this->candidate_index + 1 ? U">" : U" ";
    candidate_display += this->candidates[i];
  }
  if (this->candidates.size() == this->candidate_index + 1) {
    candidate_display += U">";
  }

  WriteUTF32String(*box.text_window->InnerWriter(), Vector2D<int>{4, 6 + 17 + 2}, candidate_display.c_str(), ToColor(0));
}

void InputTextWindowPekzep(CursoredTextBox& box, char32_t unicode, uint8_t modifier, uint8_t keycode) {
  static IMEState state = {
    {U'我', U'心', U'口'},
    {U'k', U'a', U'r'},
  };


  if (keycode == 79 /* RightArrow */) {
    state.candidate_index++; // todo: error check
    state.Render(box);
  } else if (keycode == 80 /* LeftArrow */) {
    state.candidate_index--; // todo: error check
    state.Render(box);
  } else if (unicode == U'\b') {
    box.DrawTextCursor(false);
    if (!state.non_solidified.empty()) {
      state.non_solidified.pop_back();
    } else if (!state.solidified.empty()) {
      state.solidified.pop_back();
    }
    state.Render(box);
    box.DrawTextCursor(true);
  } else if (U'a' <= unicode && unicode <= U'z') {
    state.non_solidified.push_back(unicode);
    state.Render(box);
  }

  if (unicode == 0) {
    return;
  }

/*
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
*/
  layer_manager->Draw(box.text_window_layer_id);
}
