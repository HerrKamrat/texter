//
//  text.hpp
//  texter
//
//  Created by Gabriel on 2019-07-03.
//
#pragma once

#include <string>
#include <vector>

#include <hb.h>

enum class Encoding {
    Utf8,
    Utf16,
    Utf32,
};

enum class Direction {
    Invalid = 0,
    LTR = 4,
    RTL,
    // TTB,
    // BTT,
};

using text_index_t = int;
using text_length_t = int;

struct Point {
    hb_position_t x;
    hb_position_t y;
};

struct Size {
    hb_position_t width;
    hb_position_t height;
};

struct Rect {
    Point position;
    Size size;
};

struct GlyphInfo {
    uint32_t codepoint;
    uint32_t cluster;
    Point advance;
    Point offset;
};

class TextFont {
  public:
  //protected:
  //private:
      hb_font_t* font;
      
};

class TextParagraph {
  public:
    // protected:
    // private:
    const text_index_t offset;
    const text_length_t length;
};

class TextRun {
  public:
    // protected:
    // private:
    const text_index_t offset;
    const text_length_t length;
    const Direction direction;

    Rect bounds;
};

class TextBlob {
  public:

    void debug() const;

    // protected:
    // private:
    void set(const std::string& text, Encoding encoding = Encoding::Utf8);

    void findBreaks();

    void shape(const TextFont& font);

    uint32_t m_key;
    uint32_t m_size;

    std::vector<uint32_t> m_codepoints;
    std::vector<GlyphInfo> m_infos;
    std::vector<TextParagraph> m_paragraphs;
    std::vector<TextRun> m_runs;

    std::vector<char> m_wordbreaks;
    std::vector<char> m_graphemebreaks;
};


class TextShaper {
    void shape(const TextBlob& blob);
};