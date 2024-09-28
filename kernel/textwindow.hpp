#pragma once
#include "cursored_textbox.hpp"
void InitializeTextWindow(PrivilegedCursoredTextBox& box, int win_w, int win_h, const char *title, Vector2D<int> pos);
void DrawTextCursor(PrivilegedCursoredTextBox& box, bool visible);
void InputTextWindow(PrivilegedCursoredTextBox& box, char32_t unicode);
