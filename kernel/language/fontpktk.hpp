#pragma once

#include <cstdint>
#include <utility>


#include "graphics.hpp"
#include "error.hpp"

unsigned long WriteStringInPektak(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color, uint16_t font_height);
void InitializePektakFont();
