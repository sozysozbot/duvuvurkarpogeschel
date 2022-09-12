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
#include "builtin_textbox.hpp"

void BuiltInTextBox::SetTimer(int timer_id, double timer_sec)
{
	cursorTimer = timer_id;
	kTimer = static_cast<unsigned long>(kTimerFreq * 0.5);
	timer_manager->AddTimer(Timer{kTimer, cursorTimer, 1});
}
