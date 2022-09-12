#include <cstdint>
#include <cstddef>
#include <limits>
#include <numeric>
#include <vector>

#include "font.hpp"
#include "memory_manager.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "terminal.hpp"
#include "textwindowbhat.hpp"
#include "syscall.hpp"
#include "keyboard.hpp"
#include "builtin_textbox.hpp"

struct Keymap {
  char32_t keycode;
  char32_t no_modifier;
  char32_t first_modifier;
  char32_t second_modifier;
};

Keymap keymap[] ={
  {U'p',	U'\u092a'	/*p*/,	0,	U'\u092b'/*ph*/,},
  {U'b',	U'\u092c'	/*b*/,	0,	U'\u092d'/*bh*/,},
  {U'm',	U'\u092e'	/*m*/},
  {U'c',	U'\u0938'	/*c*/,	U'\u0937' /*ṣ*/,	},
  {U's',	U'\u095b'	/*s*/},
  {U'x',	U'\u0936'	/*x*/},
  {U'z',	U'\u091a'	/*z*/},
  {U't',	U'\u0924'	/*t*/,	U'\u091f' /*ṭ*/},
  {U'd',	U'\u0926'	/*d*/,	U'\u0921' /*ḍ*/,	U'\u0927'/*dh*/,},
  {U'n',	U'\u0928'	/*n*/,	U'\u0923' /*ṇ*/},
  {U'l',	U'\u0932'	/*l*/,	U'\u0933' /*ḷ*/},
  {U'r',	U'\u0930'	/*r*/},
  {U'k',	U'\u0915'	/*k*/,	0,	U'\u0916'/*kh*/},
  {U'g',	U'\u0917'	/*g*/,	0,	U'\u095a'/*gh*/},
  {U'h',	U'\u0959'	/*h*/},
  {U'j',	U'\u092f'	/*j*/},
  {U'y',	U'\u095f'	/*y*/},
  {U'w',	U'\u0935'	/*w*/},
  {U'a',	U'\u0905'	/*a*/,	U'\u0985'/*at*/,	U'\u0906'/*á*/},
  {U'e',	U'\u090f'	/*e*/,	0,	U'\u0910'/*ai*/},
  {U'i',	U'\u0907'	/*i*/,	U'\u0987'/*it*/,	U'\u0908'/*í*/},
  {U'u',	U'\u0909'	/*u*/,	U'\u0989'/*ut*/,	U'\u090a'/*ú*/},
  {U'o',	U'\u0913'	/*o*/,	U'\u0994'/*aut*/,	U'\u0914'/*au*/},
  {U'"',	U'\u201e'	/*"*/,	U'\u2836'/*2quot*/,	U'\u283f'/*3quot*/},
  {U',',	U'\u0964'	/*,*/},
  {U'.',	U'\u0965'	/*.*/},
  {U'&',	U'\u10fb'	/*¶*/},
  {U'_',	U'\u203e'	/*_*/},
  {U'?',	U'\u2e2e'	/*?*/},
  {U' ',	U' '}
};

bool isCtrlPressed(uint8_t modifier) {
  return modifier & (kLControlBitMask | kRControlBitMask);
}

bool isAltPressed(uint8_t modifier) {
  return modifier & (kLAltBitMask | kRAltBitMask);
}

void InputTextWindowBhat(BuiltInTextBox& box, char32_t unicode, uint8_t modifier) {
  static std::vector<char32_t> content;
  if (unicode == 0) {
    return;
  }

  auto pos = [&box]() { return Vector2D<int>{4 + 8*box.text_window_index, 6}; };

  const int max_chars = (box.text_window->InnerSize().x - 8) / 8 - 1;
  if (unicode == U'\b' && box.text_window_index > 0) {
    box.DrawTextCursor(false);
    --box.text_window_index;
    FillRectangle(*box.text_window->InnerWriter(), pos(), {8, 16}, ToColor(0xffffff));
    if (!content.empty()) {
      content.pop_back();
    }
    box.DrawTextCursor(true);
  } else if (unicode >= ' ' && box.text_window_index < max_chars) {
    box.DrawTextCursor(false);
    char32_t c = 0;
    for (int i = 0; i < sizeof keymap / sizeof *keymap; i++) {
      if (keymap[i].keycode == unicode) {
        // First modifier == Ctrl
        // Second modifier == Alt
        if (isCtrlPressed(modifier)) {
          if (isAltPressed(modifier)) {
            // Ignore when both modifiers are pressed
          } else {
            c = keymap[i].first_modifier;
          }
        } else {
          if (isAltPressed(modifier)) {
            c = keymap[i].second_modifier;
          } else {
            c = keymap[i].no_modifier;
          }
        } 
      }
    }

    if (c) {
      // When the previous character is a consonant, key "A" should give a halant (aumakátá rod).
      // 直前が子音字であるなら、A キーはハラント（アウマカーター・ロード）を出さねばならない。
      if (c == U'\u0905' && !content.empty()) {
        switch (content.back()) {
          case U'\u092a' /*p*/: case U'\u092b'/*ph*/: case U'\u092c' /*b*/:  
          case U'\u092d'/*bh*/: case U'\u092e' /*m*/: case U'\u0938' /*c*/: 
          case U'\u0937' /*ṣ*/: case U'\u095b' /*s*/: case U'\u0936' /*x*/: 
          case U'\u091a' /*z*/: case U'\u0924' /*t*/: case U'\u091f' /*ṭ*/: 
          case U'\u0926' /*d*/: case U'\u0921' /*ḍ*/: case U'\u0927'/*dh*/: 
          case U'\u0928' /*n*/: case U'\u0923' /*ṇ*/: case U'\u0932' /*l*/: 
          case U'\u0933' /*ḷ*/: case U'\u0930' /*r*/: case U'\u0915' /*k*/: 
          case U'\u0916'/*kh*/: case U'\u0917' /*g*/: case U'\u095a'/*gh*/: 
          case U'\u0959' /*h*/: case U'\u092f' /*j*/: case U'\u095f' /*y*/: 
          case U'\u0935' /*w*/: 
          c = U'\u094d' /*ъ*/;
        }
      }

      // When the previous character is a question mark, the double quotation key should create a ligature
      // 直前が疑問符である場合は、二重引用符はリガチャを生まなければならない。
      if (c == U'\u201e' /*"*/ && !content.empty() && content.back() == U'\u2e2e'	/*?*/) {

        // delete the previous question mark
        --box.text_window_index;
        FillRectangle(*box.text_window->InnerWriter(), pos(), {8, 16}, ToColor(0xffffff));
        content.pop_back();

        c = U'\u2047'; /* the ligature of the question mark and the double quote */
      }

      WriteUnicodeChar(*box.text_window->InnerWriter(), pos(), c, ToColor(0));
      content.push_back(c);
      ++box.text_window_index;
    }
    box.DrawTextCursor(true);
  }

  layer_manager->Draw(box.text_window_layer_id);
}
