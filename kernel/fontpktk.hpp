#pragma once

#include <cstdint>
#include <utility>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics.hpp"
#include "error.hpp"

void WriteStringInPektak(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color);

WithError<FT_Face> NewFTFaceFromPektak();
Error WriteUnicodeCharInPektak(PixelWriter& writer, Vector2D<int> pos,
                   char32_t c, const PixelColor& color);
void InitializePektakFont();
