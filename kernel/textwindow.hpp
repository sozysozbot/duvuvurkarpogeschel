#pragma once
#include "cursored_textbox.hpp"
void InitializeTextWindow(CursoredTextBox& box, int win_w, int win_h, const char *title, Vector2D<int> pos);
void DrawTextCursor(CursoredTextBox& box, bool visible);
void InputTextWindow(CursoredTextBox& box, char32_t unicode);
