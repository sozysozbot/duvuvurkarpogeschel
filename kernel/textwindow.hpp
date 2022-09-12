#pragma once
#include "builtin_textbox.hpp"
void InitializeTextWindow(BuiltInTextBox& box, int win_w, int win_h, const char *title, Vector2D<int> pos);
void DrawTextCursor(BuiltInTextBox& box, bool visible);
void InputTextWindow(BuiltInTextBox& box, char32_t unicode);
