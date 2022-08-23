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

/**There is a discrepancy between the internal encoding of duvuvurkarpogeschel
 * and the glyph table that pek_tak.ttf uses.
 * this function absorbs the discrepancy between them.
 **/
char32_t duvuv_to_pektak(char32_t duvuv) {
  switch(duvuv) {
    case U',': return U';';
    case U'.': return U':';
    case U'a': return U'A';
    case U'b': return U'B';
    case U'e': return U'E';
    case U'i': return U'I';
    case U'j': return U'J';
    case U'l': return U'L';
    case U'm': return U'M';
    case U'n': return U'N';
    case U'o': return U'O';
    case U'p': return U'P';
    case U's': return U'C';
    case U't': return U'T';
    case U'u': return U'U';
    case U'w': return U'W';
    case U'д': return U'D';
    case U'к': return U'K';
    case U'ц': return U'Z';
    case U'ш': return U'X';
    case U'。': return U'.';
    case U'々': return U'\'';
    case U'ぁ': return U'ぁ';
    case U'あ': return U'あ';
    case U'ぃ': return U'ぃ';
    case U'い': return U'い';
    case U'ぅ': return U'ぅ';
    case U'う': return U'う';
    case U'ぇ': return U'ぇ';
    case U'え': return U'え';
    case U'ぉ': return U'ぉ';
    case U'お': return U'お';
    case U'か': return U'か';
    case U'が': return U'が';
    case U'き': return U'き';
    case U'ぎ': return U'ぎ';
    case U'く': return U'く';
    case U'ぐ': return U'ぐ';
    case U'け': return U'け';
    case U'げ': return U'げ';
    case U'こ': return U'こ';
    case U'ご': return U'ご';
    case U'さ': return U'さ';
    case U'ざ': return U'ざ';
    case U'し': return U'し';
    case U'じ': return U'じ';
    case U'す': return U'す';
    case U'ず': return U'ず';
    case U'せ': return U'せ';
    case U'ぜ': return U'ぜ';
    case U'そ': return U'そ';
    case U'ぞ': return U'ぞ';
    case U'た': return U'た';
    case U'だ': return U'だ';
    case U'ち': return U'ち';
    case U'ぢ': return U'ぢ';
    case U'っ': return U'っ';
    case U'つ': return U'つ';
    case U'づ': return U'づ';
    case U'て': return U'て';
    case U'で': return U'で';
    case U'と': return U'と';
    case U'ど': return U'ど';
    case U'な': return U'な';
    case U'に': return U'に';
    case U'ぬ': return U'ぬ';
    case U'ね': return U'ね';
    case U'の': return U'の';
    case U'ば': return U'ば';
    case U'ぱ': return U'ぱ';
    case U'び': return U'び';
    case U'ぴ': return U'ぴ';
    case U'ぶ': return U'ぶ';
    case U'ぷ': return U'ぷ';
    case U'べ': return U'べ';
    case U'ぺ': return U'ぺ';
    case U'ぼ': return U'ぼ';
    case U'ぽ': return U'ぽ';
    case U'ま': return U'ま';
    case U'み': return U'み';
    case U'む': return U'む';
    case U'め': return U'め';
    case U'も': return U'も';
    case U'や': return U'や';
    case U'ゆ': return U'ゆ';
    case U'よ': return U'よ';
    case U'ら': return U'ら';
    case U'り': return U'り';
    case U'る': return U'る';
    case U'れ': return U'れ';
    case U'ろ': return U'ろ';
    case U'わ': return U'わ';
    case U'ゐ': return U'ゐ';
    case U'ゑ': return U'ゑ';
    case U'ん': return U'ん';
    case U'ー': return U'ー';
    case U'一': return U'1';
    case U'七': return U'7';
    case U'三': return U'3';
    case U'上': return U'+';
    case U'下': return U'-';
    case U'九': return U'9';
    case U'二': return U'2';
    case U'五': return U'5';
    case U'八': return U'8';
    case U'六': return U'6';
    case U'兵': return U'兵';
    case U'四': return U'4';
    case U'将': return U'将';
    case U'巫': return U'巫';
    case U'弓': return U'弓';
    case U'無': return U'無';
    case U'王': return U'王';
    case U'皇': return U'_';
    case U'筆': return U'筆';
    case U'船': return U'船';
    case U'虎': return U'虎';
    case U'車': return U'車';
    case U'銭': return U'$';
    case U'馬': return U'馬';
    case U'！': return U'!';
    case U'＂': return U'\"';
    case U'？': return U'?';
    case U'ａ': return U'a';
    case U'ｂ': return U'b';
    case U'ｃ': return U'c';
    case U'ｄ': return U'd';
    case U'ｅ': return U'e';
    case U'ｇ': return U'g';
    case U'ｈ': return U'h';
    case U'ｉ': return U'i';
    case U'ｋ': return U'k';
    case U'ｌ': return U'l';
    case U'ｍ': return U'm';
    case U'ｎ': return U'n';
    case U'ｏ': return U'o';
    case U'ｐ': return U'p';
    case U'ｓ': return U's';
    case U'ｔ': return U't';
    case U'ｕ': return U'u';
    case U'ｘ': return U'x';
    case U'ｙ': return U'y';
    case U'ｚ': return U'z';
    default: return duvuv;
  }
}

Error RenderUnicodePektak(char32_t duvuv, FT_Face face) {
  const auto glyph_index = FT_Get_Char_Index(face, duvuv_to_pektak(duvuv));
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
    WriteUnicodeCharInPektak(writer, pos + Vector2D<int>{8 * x, 0}, u32, color);
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
