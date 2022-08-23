#pragma once

#include <cstdint>
#include <utility>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics.hpp"
#include "error.hpp"

int WriteStringInPektak(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color, uint16_t font_height);

WithError<FT_Face> NewFTFaceFromPektak();
void InitializePektakFont();
