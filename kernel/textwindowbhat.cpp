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
#include "keyboard.hpp"

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

void InputTextWindowBhat(char32_t unicode, uint8_t modifier) {
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
    for (int i = 0; i < sizeof keymap / sizeof *keymap; i++) {
      if (keymap[i].keycode == unicode) {
        // First modifier == Ctrl
        // Second modifier == Alt
        if (isCtrlPressed(modifier)) {
          if (isAltPressed(modifier)) {
            // Ignore when both modifiers are pressed
          } else {
            WriteUnicodeChar(*text_window_bhat->InnerWriter(), pos(), keymap[i].first_modifier, ToColor(0));
            ++text_window_bhat_index;
            break;
          }
        } else {
          if (isAltPressed(modifier)) {
            WriteUnicodeChar(*text_window_bhat->InnerWriter(), pos(), keymap[i].second_modifier, ToColor(0));
            ++text_window_bhat_index;
            break;
          } else {
            WriteUnicodeChar(*text_window_bhat->InnerWriter(), pos(), keymap[i].no_modifier, ToColor(0));
            ++text_window_bhat_index;
            break;
          }
        } 
      }
    }
    DrawTextCursorBhat(true);
  }

  layer_manager->Draw(text_window_bhat_layer_id);
}
