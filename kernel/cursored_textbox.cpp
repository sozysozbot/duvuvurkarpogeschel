#include "window.hpp"
#include "layer.hpp"
#include "message.hpp"
#include "timer.hpp"
#include "cursored_textbox.hpp"

void CursoredTextBox::SetTimer(int timer_id, double timer_sec)
{
	cursorTimer = timer_id;
	kTimer = static_cast<unsigned long>(kTimerFreq * 0.5);
	timer_manager->AddTimer(Timer{kTimer, cursorTimer, 1});
}

void CursoredTextBox::InitializeTextWindow(int win_w, int win_h, const char *title, Vector2D<int> pos, int bottom_additional_margin) {
  this->text_window = std::make_shared<ToplevelWindow>(
      win_w, win_h, screen_config.pixel_format, title);
  Vector2D<int> bottom{0, bottom_additional_margin};
  DrawTextbox(*this->text_window->InnerWriter(), {0, 0}, this->text_window->InnerSize() - bottom);

  this->text_window_layer_id = layer_manager->NewLayer()
    .SetWindow(this->text_window)
    .SetDraggable(true)
    .Move(pos)
    .ID();

  layer_manager->UpDown(this->text_window_layer_id, std::numeric_limits<int>::max());
}

void CursoredTextBox::DrawTextCursor(bool visible) {
  const auto color = visible ? ToColor(0) : ToColor(0xffffff);
  const auto pos = Vector2D<int>{4 + 8*this->cursor_index, 5};
  FillRectangle(*this->text_window->InnerWriter(), pos, {7, 15}, color);
}

void CursoredTextBox::ManageCursor(std::optional<Message> msg) {
  __asm__("cli");
  timer_manager->AddTimer(
      Timer{msg->arg.timer.timeout + this->kTimer, this->cursorTimer, 1});
  __asm__("sti");
  this->cursor_visible = !this->cursor_visible;
  this->DrawTextCursor(this->cursor_visible);
  layer_manager->Draw(this->text_window_layer_id);
}