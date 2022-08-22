/**
 * @file fontpktk.cpp
 *
 * pek_tak フォント描画のためのファイル.
 */

#include "fontpktk.hpp"
#include "font.hpp"

#include <cstdlib>
#include <vector>
#include <bit>
#include <algorithm>

#include "fat.hpp"

namespace {
FT_Library ft_library;
std::vector<uint8_t>* pektak_buf;

Error RenderUnicode(char32_t c, FT_Face face) {
  const auto glyph_index = FT_Get_Char_Index(face, c);
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

void WriteStringInPektak(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color) {
  int x = 0;
  while (*s) {
    const auto [ u32, bytes ] = ConvertUTF8To32(s);
    WriteUnicodeChar(writer, pos + Vector2D<int>{8 * x, 0}, u32, color);
    s += bytes;
    x += IsHankaku(u32) ? 1 : 2;
  }
}

WithError<FT_Face> NewFTFaceFromPektak() {
  FT_Face face;
  if (int err = FT_New_Memory_Face(
        ft_library, pektak_buf->data(), pektak_buf->size(), 0, &face)) {
    return { face, MAKE_ERROR(Error::kFreeTypeError) };
  }
  if (int err = FT_Set_Pixel_Sizes(face, 16, 16)) {
    return { face, MAKE_ERROR(Error::kFreeTypeError) };
  }
  return { face, MAKE_ERROR(Error::kSuccess) };
}

/**
 * 埋める領域は動的に決まるのよなぁ
 */
Error WriteUnicodeCharInPektak(PixelWriter& writer, Vector2D<int> pos,
                  char32_t c, const PixelColor& color) {
  auto [ face, err ] = NewFTFaceFromPektak();
  if (err) {
    return err;
  }
  if (auto err = RenderUnicode(c, face)) {
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

  FT_Done_Face(face);
  return MAKE_ERROR(Error::kSuccess);
}

void InitializePektakFont() {
  if (int err = FT_Init_FreeType(&ft_library)) {
    exit(1);
  }

  auto [ entry, pos_slash ] = fat::FindFile("/__pektak.ttf");
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
