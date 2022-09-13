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
  void Render(CursoredTextBox& box, bool non_solidified_is_updated);
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
  this->candidate_index = 0;
}

void IMEState::Render(CursoredTextBox& box, bool non_solidified_is_updated) {
  // first, erase everything
  box.ClearTextWindow();

  // then render
  const int max_width = (box.text_window->InnerSize().x - 8) / 8 - 1;

  box.DrawTextCursor(false);
  int solidified_width = WriteUTF32CharVecWithUpperLimit(
    *box.text_window->InnerWriter(), 
    Vector2D<int>{4, 6}, 
    this->solidified, 
    ToColor(0),
    max_width
  );
  int unsolidified_width = WriteUTF32CharVecWithUpperLimit(
    *box.text_window->InnerWriter(), 
    Vector2D<int>{4 + 8 * solidified_width, 6}, 
    this->non_solidified, 
    ToColor(0xe916c3),
    max_width - solidified_width
  );
  box.cursor_index = solidified_width + unsolidified_width;
  box.DrawTextCursor(true);

  if (non_solidified_is_updated) {
    ComputeCandidatesAndStore();
  }
  // prepares a string of the form U"<此>時 火 車 善 子"
  std::u32string candidate_display = U"";
  for (int i = 0; i < this->candidates.size(); i++) {
    candidate_display += i == this->candidate_index ? U"<" : i == this->candidate_index + 1 ? U">" : U" ";
    candidate_display += this->candidates[i];
  }
  if (this->candidates.size() == this->candidate_index + 1) {
    candidate_display += U">";
  }

  WriteUTF32StringWithUpperLimit(
    *box.text_window->InnerWriter(), 
    Vector2D<int>{4, 6 + 17 + 2}, 
    candidate_display.c_str(), 
    ToColor(0),
    max_width
  );
}

void InputTextWindowPekzep(CursoredTextBox& box, char32_t unicode, uint8_t modifier, uint8_t keycode) {
  static IMEState state;

  if (keycode == 79 /* RightArrow */) {
    state.candidate_index++; // todo: error check
    state.Render(box, false);
  } else if (keycode == 80 /* LeftArrow */) {
    state.candidate_index--; // todo: error check
    state.Render(box, false);
  } else if (unicode == U'\b') {
    box.DrawTextCursor(false);
    if (!state.non_solidified.empty()) {
      state.non_solidified.pop_back();
      state.Render(box, true);
    } else if (!state.solidified.empty()) {
      state.solidified.pop_back();
      state.Render(box, false);
    }
    box.DrawTextCursor(true);
  } else if (U'a' <= unicode && unicode <= U'z') {
    state.non_solidified.push_back(unicode);
    state.Render(box, true);
  } else if (unicode == U' ') {
    if (state.non_solidified.empty()) {
      state.solidified.push_back(U' ');
    } else {
      auto new_str = state.candidates[state.candidate_index];
      state.solidified.insert(state.solidified.end(), new_str.begin(), new_str.end());
      state.non_solidified.clear();
      state.Render(box, true);
    }
  }

  if (unicode == 0) {
    return;
  }

  layer_manager->Draw(box.text_window_layer_id);
}
