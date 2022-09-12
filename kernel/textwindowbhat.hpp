#pragma once
void InitializeTextWindowBhat(BuiltInTextBox& box, int win_w, int win_h, const char *title, Vector2D<int> pos);
void DrawTextCursorBhat(bool visible);
void InputTextWindowBhat(char32_t unicode, uint8_t modifier);
extern unsigned int text_window_bhat_layer_id;
