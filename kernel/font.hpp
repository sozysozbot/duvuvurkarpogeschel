#pragma once

#include <cstdint>
#include <utility>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "graphics.hpp"
#include "error.hpp"

void WriteAscii(PixelWriter& writer, Vector2D<int> pos, char c, const PixelColor& color);
int WriteUTF8String(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color);
int WriteUTF32String(PixelWriter& writer, Vector2D<int> pos, const char32_t* s, const PixelColor& color);
int WriteUTF32StringWithUpperLimit(PixelWriter& writer, Vector2D<int> pos, const char32_t* s, const PixelColor& color, int max_width);
int WriteUTF32CharVec(PixelWriter& writer, Vector2D<int> pos, const std::vector<char32_t>& vec, const PixelColor& color);
int WriteUTF32CharVecWithUpperLimit(PixelWriter& writer, Vector2D<int> pos, const std::vector<char32_t>& vec, const PixelColor& color, int max_width);
int CountUTF8Size(uint8_t c);
std::pair<char32_t, int> ConvertUTF8To32(const char* u8);
bool IsHankaku(char32_t c);
WithError<FT_Face> NewFTFace();
Error WriteUnicodeChar(PixelWriter& writer, Vector2D<int> pos,
                   char32_t c, const PixelColor& color);
void InitializeFont();
