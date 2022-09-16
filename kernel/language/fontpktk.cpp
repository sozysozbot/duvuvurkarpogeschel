/**
 * @file fontpktk.cpp
 *
 * pek_tak フォント描画のためのファイル.
 */

#include "language/fontpktk.hpp"
#include "language/font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdlib>
#include <vector>
#include <bit>
#include <algorithm>

#include "fat.hpp"

namespace {
FT_Library ft_library;
std::vector<uint8_t>* pektak_buf;

Error RenderUnicodePektak(char32_t pektak, FT_Face face) {
  const auto glyph_index = FT_Get_Char_Index(face, pektak);
  if (glyph_index == 0) {
    return MAKE_ERROR(Error::kFreeTypeError);
  }

  if (int err = FT_Load_Glyph(face, glyph_index,
                              FT_LOAD_RENDER | FT_LOAD_TARGET_MONO)) {
    return MAKE_ERROR(Error::kFreeTypeError);
  }
  return MAKE_ERROR(Error::kSuccess);
}

} // namespace

Error WriteUnicodeCharInPektak(PixelWriter& writer, Vector2D<int> pos,
                  char32_t c, const PixelColor& color, uint16_t font_height, uint16_t& horizontal_advance_ref);

/**
 * @returns 文字列の横幅（単位はピクセル） / the resulting width (in pixel) of the text
 * 
 */
unsigned long WriteStringInPektak(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color, uint16_t font_height) {
  int x_in_pixel = 0;
  while (*s) {
    const auto [ u32, bytes ] = ConvertUTF8To32(s);
    uint16_t horizontal_advance = font_height; // the horizontal advance defaults to a fullwidth character
    WriteUnicodeCharInPektak(writer, pos + Vector2D<int>{x_in_pixel, 0}, u32, color, font_height, horizontal_advance);
    s += bytes;
    x_in_pixel += horizontal_advance;
  }
  return x_in_pixel;
}

WithError<FT_Face> NewFTFaceFromPektak(uint16_t font_height) {
  FT_Face face;
  if (int err = FT_New_Memory_Face(
        ft_library, pektak_buf->data(), pektak_buf->size(), 0, &face)) {
    return { face, MAKE_ERROR(Error::kFreeTypeError) };
  }
  if (int err = FT_Set_Pixel_Sizes(face, font_height, font_height)) {
    return { face, MAKE_ERROR(Error::kFreeTypeError) };
  }
  return { face, MAKE_ERROR(Error::kSuccess) };
}

/**
 * @param horizontal_advance 参照を受け取り、文字の横幅を返す
 */
Error WriteUnicodeCharInPektak(PixelWriter& writer, Vector2D<int> pos,
                  char32_t c, const PixelColor& color, uint16_t font_height, uint16_t& horizontal_advance_ref) {
  auto [ face, err ] = NewFTFaceFromPektak(font_height);
  if (err) {
    return err;
  }
  if (auto err = RenderUnicodePektak(c, face)) {
    return err;
  }
  FT_Bitmap& bitmap = face->glyph->bitmap;

  const int baseline = (face->height + face->descender) *
    face->size->metrics.y_ppem / face->units_per_EM;
  const auto glyph_topleft = pos + Vector2D<int>{
    face->glyph->bitmap_left, baseline - face->glyph->bitmap_top};

  for (int dy = 0; dy < bitmap.rows; ++dy) {
    unsigned char* q = &bitmap.buffer[bitmap.pitch * dy];
    if (bitmap.pitch < 0) {
      q -= bitmap.pitch * bitmap.rows;
    }
    for (int dx = 0; dx < bitmap.width; ++dx) {
      const bool b = q[dx >> 3] & (0x80 >> (dx & 0x7));
      if (b) {
        writer.Write(glyph_topleft + Vector2D<int>{dx, dy}, color);
      }
    }
  }

  horizontal_advance_ref = face->glyph->advance.x >> 6;  
  FT_Done_Face(face);
  return MAKE_ERROR(Error::kSuccess);
}

void InitializePektakFont() {
  if (int err = FT_Init_FreeType(&ft_library)) {
    exit(1);
  }

  auto [ entry, pos_slash ] = fat::FindFile("/__duvuv.ttf");
  if (entry == nullptr || pos_slash) {
    exit(1);
  }

  const size_t size = entry->file_size;
  pektak_buf = new std::vector<uint8_t>(size);
  if (LoadFile(pektak_buf->data(), size, *entry) != size) {
    delete pektak_buf;
    exit(1);
  }
}
