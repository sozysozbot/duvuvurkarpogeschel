/**
 * @file main.cpp
 *
 * カーネル本体のプログラムを書いたファイル．
 */

#include <cstdint>
#include <cstddef>
#include <cstdio>

#include <deque>
#include <limits>
#include <numeric>
#include <vector>

#include "frame_buffer_config.hpp"
#include "memory_map.hpp"
#include "graphics.hpp"
#include "mouse.hpp"
#include "font.hpp"
#include "fontpktk.hpp"
#include "console.hpp"
#include "pci.hpp"
#include "logger.hpp"
#include "usb/xhci/xhci.hpp"
#include "interrupt.hpp"
#include "asmfunc.h"
#include "segment.hpp"
#include "paging.hpp"
#include "memory_manager.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "message.hpp"
#include "timer.hpp"
#include "acpi.hpp"
#include "keyboard.hpp"
#include "task.hpp"
#include "terminal.hpp"
#include "fat.hpp"
#include "syscall.hpp"
#include "osbanner.h"
#include "textwindow.hpp"
#include "textwindowbhat.hpp"
#include "cursored_textbox.hpp"

int printk(const char* format, ...) {
  va_list ap;
  int result;
  char s[1024];

  va_start(ap, format);
  result = vsprintf(s, format, ap);
  va_end(ap);

  console->PutString(s);
  return result;
}

std::shared_ptr<ToplevelWindow> counter_window;
unsigned int counter_window_layer_id;
void InitializeCounterWindow() {
  counter_window = std::make_shared<ToplevelWindow>(
      160, 52, screen_config.pixel_format, "kinfiter");

  counter_window_layer_id = layer_manager->NewLayer()
    .SetWindow(counter_window)
    .SetDraggable(true)
    .Move({300, 100})
    .ID();

  layer_manager->UpDown(counter_window_layer_id, std::numeric_limits<int>::max());
}

alignas(16) uint8_t kernel_main_stack[1024 * 1024];

/** 本当は画面縦幅は horizontal_resolution、画面横幅は vertical_resolution なのだが、
 *  768 x 543 の固定幅の画面をエミュレートする。
 *  ただし、それはそれとして枠外に固定のバナーは描きたい。
 *  バナーは一度描画したら上書きされることはないので、ここで描画しておく。
 */
FrameBufferConfig DrawBannerAndShrinkScreenTo768x543(const FrameBufferConfig& old_config) {
  const uint32_t FIXED_HORIZONTAL_RES = 768;
  const uint32_t FIXED_VERTICAL_RES = 543;
  assert(old_config.horizontal_resolution >= FIXED_HORIZONTAL_RES);
  assert(old_config.vertical_resolution >= FIXED_VERTICAL_RES);

  // I want `margin_left` to be equal to `margin_right`
  auto margin_left = (old_config.horizontal_resolution - FIXED_HORIZONTAL_RES) / 2;

  // I want `margin_bottom` to be twice the size of `margin_top`
  auto margin_top = (old_config.vertical_resolution - FIXED_VERTICAL_RES) / 3;

  // I intend to show the banner within the bottom margin.
  auto y_start = FIXED_VERTICAL_RES + margin_top;

  // std::min is used to account for the case where the banner's height is too large.
  auto y_end = std::min(y_start + osbanner.height, old_config.vertical_resolution);
  int x_center = FIXED_HORIZONTAL_RES / 2 + margin_left; 

  int x_left = x_center - osbanner.width / 2;
  int x_right = x_left + osbanner.width;
  int x_start = std::max(x_left, 0);
  int x_end = std::min(x_right, static_cast<int>(old_config.horizontal_resolution));

  for (int y = y_start; y < y_end; ++y) {
    for (int x = x_start; x < x_end; ++x) {
      const uint8_t *p = &osbanner.pixel_data[(osbanner.width * (y - y_start) + (x - x_left)) * osbanner.bytes_per_pixel];
      PrimitivelyWritePixel(old_config, x, y, {p[0], p[1], p[2]});
    }
  }

  FrameBufferConfig new_config = {
    old_config.frame_buffer 
      + BytesPerPixel(old_config.pixel_format) * (old_config.pixels_per_scan_line * margin_top + margin_left),
    old_config.pixels_per_scan_line,
    FIXED_HORIZONTAL_RES,
    FIXED_VERTICAL_RES,
    old_config.pixel_format
  };
    
  return new_config;
}

void ManageCursor(CursoredTextBox &box, std::optional<Message> msg) {
  __asm__("cli");
  timer_manager->AddTimer(
      Timer{msg->arg.timer.timeout + box.kTimer, box.cursorTimer, 1});
  __asm__("sti");
  box.cursor_visible = !box.cursor_visible;
  box.DrawTextCursor(box.cursor_visible);
  layer_manager->Draw(box.text_window_layer_id);
}

extern "C" void KernelMainNewStack(
    const FrameBufferConfig& physical_frame_buffer_config_ref,
    const MemoryMap& memory_map_ref,
    const acpi::RSDP& acpi_table,
    void* volume_image) {
  MemoryMap memory_map{memory_map_ref};
  const FrameBufferConfig shrunk_frame_buffer_config = DrawBannerAndShrinkScreenTo768x543(physical_frame_buffer_config_ref);
  InitializeGraphics(shrunk_frame_buffer_config);
  InitializeConsole();

  printk("xux el duvuvurkarpogeschel!\n");
  printk("krante \"kanti\" fal ksf fua quneo liusel.\n");
  SetLogLevel(kWarn);

  InitializeSegmentation();
  InitializePaging();
  InitializeMemoryManager(memory_map);
  InitializeTSS();
  InitializeInterrupt();

  fat::Initialize(volume_image);
  InitializeFont();
  InitializePektakFont();
  InitializePCI();

  InitializeLayer();
  InitializeCounterWindow();

  // pertinent to textwindow.cpp
  CursoredTextBox normal_text_window;
  normal_text_window.InitializeTextWindow(168, 52, "slahurfaesal", {500, 100});

  // pertinent to textwindowbhat.cpp
  CursoredTextBox bhat_text_window;
  bhat_text_window.InitializeTextWindow(368, 52, "phertarsvirle'i slahurfaesal", {300, 45});

  layer_manager->Draw({{0, 0}, ScreenSize()});

  acpi::Initialize(acpi_table);
  InitializeLAPICTimer();

  normal_text_window.SetTimer(1, 0.5);
  bhat_text_window.SetTimer(2, 0.5);

  InitializeSyscall();

  InitializeTask();
  Task& main_task = task_manager->CurrentTask();

  usb::xhci::Initialize();
  InitializeKeyboard();
  InitializeMouse();

  app_loads = new std::map<fat::DirectoryEntry*, AppLoadInfo>;
  task_manager->NewTask()
    .InitContext(TaskTerminal, 0)
    .Wakeup();

  char str[128];

  while (true) {
    __asm__("cli");
    const auto tick = timer_manager->CurrentTick();
    __asm__("sti");

    sprintf(str, "%010lu", tick);
    FillRectangle(*counter_window->InnerWriter(), {20, 4}, {8 * 10, 16}, {0xc6, 0xc6, 0xc6});
    WriteString(*counter_window->InnerWriter(), {20, 4}, str, {0, 0, 0});
    layer_manager->Draw(counter_window_layer_id);

    __asm__("cli");
    auto msg = main_task.ReceiveMessage();
    if (!msg) {
      main_task.Sleep();
      __asm__("sti");
      continue;
    }

    __asm__("sti");

    switch (msg->type) {
    case Message::kInterruptXHCI:
      usb::xhci::ProcessEvents();
      break;
    case Message::kTimerTimeout:
      if (msg->arg.timer.value == normal_text_window.cursorTimer) {
        ManageCursor(normal_text_window, msg);
      } else if (msg->arg.timer.value == bhat_text_window.cursorTimer) {
        ManageCursor(bhat_text_window, msg);
      }
      break;
    case Message::kKeyPush: {
      auto act = active_layer->GetActive(); 
      if (act == normal_text_window.text_window_layer_id) {
        if (msg->arg.keyboard.press) {
          InputTextWindow(normal_text_window, msg->arg.keyboard.unicode);
        }
      } else if (act == bhat_text_window.text_window_layer_id) {
        if (msg->arg.keyboard.press) {
          InputTextWindowBhat(bhat_text_window, msg->arg.keyboard.unicode, msg->arg.keyboard.modifier);
        }
      } else if (msg->arg.keyboard.press &&
                 msg->arg.keyboard.keycode == 59 /* F2 */) {
        task_manager->NewTask()
          .InitContext(TaskTerminal, 0)
          .Wakeup();
      } else {
        __asm__("cli");
        auto task_it = layer_task_map->find(act);
        __asm__("sti");
        if (task_it != layer_task_map->end()) {
          __asm__("cli");
          task_manager->SendMessage(task_it->second, *msg);
          __asm__("sti");
        } else {
          printk("slahurfaeso veles niv ferleso. lysolarakrapt m'es %d, lyjotrakrapt es %d.\n",
              msg->arg.keyboard.keycode,
              msg->arg.keyboard.unicode);
        }
      }
      break;
    }
    case Message::kLayer:
      ProcessLayerMessage(*msg);
      __asm__("cli");
      task_manager->SendMessage(msg->src_task, Message{Message::kLayerFinish});
      __asm__("sti");
      break;
    default:
      Log(kError, "Unknown message type: %d\n", msg->type);
    }
  }
}

extern "C" void __cxa_pure_virtual() {
  while (1) __asm__("hlt");
}
