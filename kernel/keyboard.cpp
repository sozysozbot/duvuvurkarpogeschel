#include "keyboard.hpp"

#include <memory>
#include "usb/classdriver/keyboard.hpp"
#include "task.hpp"

namespace {

const char32_t keycode_map[256] = {
  0,     0,     0,    0,     U'a',  U'b', U'c', U'd', // 0
  U'e',  U'f',  U'g', U'h',  U'i',  U'j', U'k', U'l', // 8
  U'm',  U'n',  U'o', U'p',  U'q',  U'r', U's', U't', // 16
  U'u',  U'v',  U'w', U'x',  U'y',  U'z', U'1', U'2', // 24
  U'3',  U'4',  U'5', U'6',  U'7',  U'8', U'9', U'0', // 32
  U'\n', U'\b', 0x08, U'\t', U' ',  U'-', U'=', U'[', // 40
  U']',  U'\\', U'#', U';',  U'\'', U'`', U',', U'.', // 48
  U'/',  0,     0,    0,     0,     0,    0,    0,   // 56
  0,     0,     0,    0,     0,     0,    0,    0,   // 64
  0,     0,     0,    0,     0,     0,    0,    0,   // 72
  0,     0,     0,    0,     U'/',  U'*', U'-', U'+', // 80
  U'\n', U'1',  U'2', U'3',  U'4',  U'5', U'6', U'7', // 88
  U'8',  U'9',  U'0', U'.',  U'\\', 0,    0,    U'=', // 96
  0,     0,     0,    0,     0,     0,    0,    0,   // 104
  0,     0,     0,    0,     0,     0,    0,    0,   // 112
  0,     0,     0,    0,     0,     0,    0,    0,   // 120
  0,     0,     0,    0,     0,     0,    0,    0,   // 128
  0,    U'\\',  0,    0,     0,     0,    0,    0,   // 136
};

const char32_t keycode_map_shifted[256] = {
 0,     0,     0,    0,     U'A',  U'B', U'ш', U'д', // 0
 U'E',  U'φ',  U'G', U'H',  U'I',  U'J', U'к', U'L', // 8
 U'M',  U'N',  U'O', U'P',  U'Q',  U'R', U'ж', U'T', // 16
 U'U',  U'β',  U'W', U'ж',  U'Y',  U'ц', U'!', U'@', // 24
 U'#',  U'$',  U'%', U'^',  U'&',  U'*', U'(', U')', // 32
 U'\n', U'\b', 0x08, U'\t', U' ',  U'_', U'+', U'{', // 40
 U'}',  U'|',  U'~', U':',  U'"',  U'~', U'<', U'>', // 48
 U'?',  0,     0,    0,     0,     0,    0,    0,    // 56
 0,     0,     0,    0,     0,     0,    0,    0,    // 64
 0,     0,     0,    0,     0,     0,    0,    0,    // 72
 0,     0,     0,    0,     U'/',  U'*', U'-', U'+', // 80
 U'\n', U'1',  U'2', U'3',  U'4',  U'5', U'6', U'7', // 88
 U'8',  U'9',  U'0', U'.',  U'\\', 0,    0,    U'=', // 96
 0,     0,     0,    0,     0,     0,    0,    0,    // 104
 0,     0,     0,    0,     0,     0,    0,    0,    // 112
 0,     0,     0,    0,     0,     0,    0,    0,    // 120
 0,     0,     0,    0,     0,     0,    0,    0,    // 128
 0,     U'|',  0,    0,     0,     0,    0,    0,    // 136
};

} // namespace

void InitializeKeyboard() {
  usb::HIDKeyboardDriver::default_observer =
    [](uint8_t modifier, uint8_t keycode, bool press) {
      const bool shift = (modifier & (kLShiftBitMask | kRShiftBitMask)) != 0;
      char32_t unicode = keycode_map[keycode];
      if (shift) {
        unicode = keycode_map_shifted[keycode];
      }
      Message msg{Message::kKeyPush};
      msg.arg.keyboard.modifier = modifier;
      msg.arg.keyboard.keycode = keycode;
      msg.arg.keyboard.unicode = unicode;
      msg.arg.keyboard.press = press;
      task_manager->SendMessage(1, msg);
    };
}
