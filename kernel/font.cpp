/**
 * @file font.cpp
 *
 * フォント描画のプログラムを集めたファイル.
 */

#include "font.hpp"

#include <cstdlib>
#include <vector>
#include <bit>
#include <algorithm>

#include "fat.hpp"

struct HalfwidthGlyph {
  uint32_t code_point;
  uint8_t glyph[16];
};

struct FullwidthGlyph {
  uint32_t code_point;
  uint16_t glyph[16];
};

extern const HalfwidthGlyph _binary_hankaku_bin_start[];
extern const HalfwidthGlyph _binary_hankaku_bin_end[];
extern const uint8_t _binary_hankaku_bin_size;

extern const FullwidthGlyph _binary_zenkaku_bin_start[];
extern const FullwidthGlyph _binary_zenkaku_bin_end[];
extern const uint8_t _binary_zenkaku_bin_size;

namespace {
FT_Library ft_library;
std::vector<uint8_t>* nihongo_buf;

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

bool CompFullwidthGlyph(const FullwidthGlyph &a, const FullwidthGlyph &b) {
  return a.code_point < b.code_point;
}

bool CompHalfwidthGlyph(const HalfwidthGlyph &a, const HalfwidthGlyph &b) {
  return a.code_point < b.code_point;
}

void WriteGlitched(PixelWriter& writer, Vector2D<int> pos, const PixelColor& color) {
  const uint8_t glyph[16] = {
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
    0b00110011,
    0b11001100,
  };
  for (int dy = 0; dy < 16; ++dy) {
    for (int dx = 0; dx < 8; ++dx) {
      if ((glyph[dy] << dx) & 0x80u) {
        writer.Write(pos + Vector2D<int>{dx, dy}, color);
      }
    }
  }
}

[[deprecated("should always write in Unicode")]]
void WriteAscii(PixelWriter& writer, Vector2D<int> pos, char c, const PixelColor& color) {
  HalfwidthGlyph gl = { static_cast<uint32_t>(c), {} };
  const HalfwidthGlyph *candidate = std::lower_bound(_binary_hankaku_bin_start, _binary_hankaku_bin_end, gl, CompHalfwidthGlyph);
  if (candidate->code_point == c) {
    const uint8_t *glyph = candidate->glyph;
    for (int dy = 0; dy < 16; ++dy) {
      for (int dx = 0; dx < 8; ++dx) {
        if ((glyph[dy] << dx) & 0x80u) {
          writer.Write(pos + Vector2D<int>{dx, dy}, color);
        }
      }
    }
  } else {
    WriteGlitched(writer, pos, color);
  }
}

void WriteString(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color) {
  int x = 0;
  while (*s) {
    const auto [ u32, bytes ] = ConvertUTF8To32(s);
    WriteUnicodeChar(writer, pos + Vector2D<int>{8 * x, 0}, u32, color);
    s += bytes;
    x += IsHankaku(u32) ? 1 : 2;
  }
}

int CountUTF8Size(uint8_t c) {
  if (c < 0x80) {
    return 1;
  } else if (0xc0 <= c && c < 0xe0) {
    return 2;
  } else if (0xe0 <= c && c < 0xf0) {
    return 3;
  } else if (0xf0 <= c && c < 0xf8) {
    return 4;
  }
  return 0;
}

std::pair<char32_t, int> ConvertUTF8To32(const char* u8) {
  switch (CountUTF8Size(u8[0])) {
  case 1:
    return {
      static_cast<char32_t>(u8[0]),
      1
    };
  case 2:
    return {
      (static_cast<char32_t>(u8[0]) & 0b0001'1111) << 6 |
      (static_cast<char32_t>(u8[1]) & 0b0011'1111) << 0,
      2
    };
  case 3:
    return {
      (static_cast<char32_t>(u8[0]) & 0b0000'1111) << 12 |
      (static_cast<char32_t>(u8[1]) & 0b0011'1111) << 6 |
      (static_cast<char32_t>(u8[2]) & 0b0011'1111) << 0,
      3
    };
  case 4:
    return {
      (static_cast<char32_t>(u8[0]) & 0b0000'0111) << 18 |
      (static_cast<char32_t>(u8[1]) & 0b0011'1111) << 12 |
      (static_cast<char32_t>(u8[2]) & 0b0011'1111) << 6 |
      (static_cast<char32_t>(u8[3]) & 0b0011'1111) << 0,
      4
    };
  default:
    return { 0, 0 };
  }
}

/**
 * @returns hankaku.bin にグリフがあるときにはそのグリフ、無いなら nullptr
 */
const HalfwidthGlyph *GetGlyphFromHankakuBin(char32_t c) {
  // Look inside the halfwidth character table
  // The table should be sorted by the code point. Hence std::lower_bound suffices.
  // return the glyph if the codepoint exactly matches
  HalfwidthGlyph gl = { c, {} };
  const HalfwidthGlyph *candidate = std::lower_bound(_binary_hankaku_bin_start, _binary_hankaku_bin_end, gl, CompHalfwidthGlyph);
  if (candidate->code_point == c) { 
    return candidate; 
  } else { 
    return nullptr; 
  }
}

/**
 * @returns zenkaku.bin にグリフがあるときにはそのグリフ、無いなら nullptr
 */
const FullwidthGlyph *GetGlyphFromZenkakuBin(char32_t c) {
  // Look inside the fullwidth character table
  // The table should be sorted by the code point. Hence std::lower_bound suffices.
  // return the glyph if the codepoint exactly matches 
  FullwidthGlyph gl = { c, {} };
  const FullwidthGlyph *candidate = std::lower_bound(_binary_zenkaku_bin_start, _binary_zenkaku_bin_end, gl, CompFullwidthGlyph);
  if (candidate->code_point == c) {
    return candidate; 
  } else { 
    return nullptr; 
  }
}

bool IsHankaku(char32_t c) {
  if (GetGlyphFromHankakuBin(c)) { 
    return true; /* 半角で表示すべき文字なので true */ 
  } else if (GetGlyphFromZenkakuBin(c)) { 
    return false; /* 全角で表示すべき文字なので false */ 
  } else if (c <= 0x7f) { 
    return true; 
    /* 禁じられたグリフ。IPAフォントが半角で表示するので、ここは true */
  } else {
    return false;
    /* 禁じられたグリフ。IPAフォントが全角で表示するので、ここは false */
  }
}

WithError<FT_Face> NewFTFace() {
  FT_Face face;
  if (int err = FT_New_Memory_Face(
        ft_library, nihongo_buf->data(), nihongo_buf->size(), 0, &face)) {
    return { face, MAKE_ERROR(Error::kFreeTypeError) };
  }
  if (int err = FT_Set_Pixel_Sizes(face, 16, 16)) {
    return { face, MAKE_ERROR(Error::kFreeTypeError) };
  }
  return { face, MAKE_ERROR(Error::kSuccess) };
}

/**
 * IsHankaku が true を返す文字なら必ず 8 x 16 の領域を、そうでなければ必ず 16 x 16 の領域を埋める。
 */
Error WriteUnicodeChar(PixelWriter& writer, Vector2D<int> pos,
                  char32_t c, const PixelColor& color) {
  if (const HalfwidthGlyph *hankaku = GetGlyphFromHankakuBin(c); hankaku) {
    const uint8_t *glyph = hankaku->glyph;
    for (int dy = 0; dy < 16; ++dy) {
      for (int dx = 0; dx < 8; ++dx) {
        if ((glyph[dy] << dx) & 0x80u) {
          writer.Write(pos + Vector2D<int>{dx, dy}, color);
        }
      }
    }
    return MAKE_ERROR(Error::kSuccess);
  }

  if (const FullwidthGlyph *zenkaku = GetGlyphFromZenkakuBin(c); zenkaku) {
    const uint16_t *glyph = zenkaku->glyph;
    for (int dy = 0; dy < 16; ++dy) {
      for (int dx = 0; dx < 16; ++dx) {
        if ((glyph[dy] << dx) & 0x8000u) {
          writer.Write(pos + Vector2D<int>{dx, dy}, color);
        }
      }
    }
    return MAKE_ERROR(Error::kSuccess);
  }

  // IPA フォントにフォールバックする。白黒反転して描画
  // fallback to the Japanese font; render the glyph with colors inverted

  auto [ face, err ] = NewFTFace();
  if (err) {
    WriteGlitched(writer, pos, color);
    if (c > 0x7f) {
      WriteGlitched(writer, pos + Vector2D<int>{8, 0}, color);
    }
    return err;
  }
  if (auto err = RenderUnicode(c, face)) {
    FT_Done_Face(face);
    WriteGlitched(writer, pos, color);
    if (c > 0x7f) {
      WriteGlitched(writer, pos + Vector2D<int>{8, 0}, color);
    }
    return err;
  }
  FT_Bitmap& bitmap = face->glyph->bitmap;

  const int baseline = (face->height + face->descender) *
    face->size->metrics.y_ppem / face->units_per_EM;

  const int background_width = c <= 0x7f ? 8 : 16;
  for (int dy = 0; dy < 16; ++dy) {
    int dy_relative = dy - (baseline - face->glyph->bitmap_top);
    if (0 <= dy_relative && dy_relative < bitmap.rows) {
      unsigned char* q = &bitmap.buffer[bitmap.pitch * dy_relative];
      if (bitmap.pitch < 0) {
        q -= bitmap.pitch * bitmap.rows;
      }

      for (int dx = 0; dx < background_width; ++dx) {
        int dx_relative = dx - face->glyph->bitmap_left;
        if (0 <= dx_relative && dx_relative < bitmap.width) {
          const bool b = q[dx_relative >> 3] & (0x80 >> (dx_relative & 0x7));
          if (!b) {
            writer.Write(pos + Vector2D<int>{dx, dy}, color);
          }
        } else {
          // outside the bounding box; always draw the background
          writer.Write(pos + Vector2D<int>{dx, dy}, color);
        }
      }
    } else {
      // outside the bounding box; always draw the background
      for (int dx = 0; dx < background_width; ++dx) {
        writer.Write(pos + Vector2D<int>{dx, dy}, color);
      }
    }
  }

  FT_Done_Face(face);
  return MAKE_ERROR(Error::kSuccess);
}

void InitializeFont() {
  if (int err = FT_Init_FreeType(&ft_library)) {
    exit(1);
  }

  auto [ entry, pos_slash ] = fat::FindFile("/__jpnipa.ttf");
  if (entry == nullptr || pos_slash) {
    exit(1);
  }

  const size_t size = entry->file_size;
  nihongo_buf = new std::vector<uint8_t>(size);
  if (LoadFile(nihongo_buf->data(), size, *entry) != size) {
    delete nihongo_buf;
    exit(1);
  }
}
