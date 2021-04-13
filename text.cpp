//
//  text.cpp
//  texter
//
//  Created by Gabriel on 2019-07-03.
//

#include "text.hpp"

#include <assert.h>
#include <limits>

#include <bx/debug.h>
#include <bx/hash.h>
#include <hb.h>
#include <utf8.h>

#include <wordbreak.h>
//#include <linebreak.h>
#include <graphemebreak.h>

#include <nlohmann/json.hpp>

extern "C" {
#include <SheenBidi.h>
}

void to_json(nlohmann::json& j, const Point& v) {
    j = nlohmann::json{{"x", v.x}, {"y", v.y}};
}

void to_json(nlohmann::json& j, const GlyphInfo& v) {
    j = nlohmann::json{
        {"codepoint", (int)v.codepoint}, {"cluster", (int)v.cluster}, {"advance", v.advance}, {"offset", v.offset}
    };
}

static_assert(Direction::Invalid == (Direction)HB_DIRECTION_INVALID);
static_assert(Direction::LTR == (Direction)HB_DIRECTION_LTR);
static_assert(Direction::RTL == (Direction)HB_DIRECTION_RTL);

Direction direction(const SBLevel& l) {
    return l % 2 ? Direction::RTL : Direction::LTR;
}

Direction direction(const hb_direction_t& l) {
    return (Direction)l;
}

template <class T>
static inline uint32_t hash(std::vector<T>& data) {
    return bx::hash<bx::HashMurmur2A>((char*)std::data(data), (size_t)std::size(data) * sizeof(T));
}

void TextBlob::set(const std::string& text, Encoding encoding) {
    auto size = utf8::distance(std::begin(text), std::end(text));

    m_codepoints.resize(size);

    auto it = utf8::utf8to32(std::begin(text), std::end(text), m_codepoints.data());

    assert(it - &m_codepoints.back() == 1);

    m_key = hash(m_codepoints);
    m_size = size;

    SBCodepointSequence seq{SBStringEncodingUTF32, (void*)m_codepoints.data(), m_codepoints.size()};
    const SBAlgorithmRef alg = SBAlgorithmCreate(&seq);

    size_t start = 0;
    int p = 0;

    Direction dir = Direction::Invalid;
    while (start < size) {
        printf("par: %d\n", p);
        ++p;
        const SBParagraphRef par = SBAlgorithmCreateParagraph(alg, start, size, SBLevelDefaultLTR);
        auto len = SBParagraphGetLength(par);
        const SBLineRef line = SBParagraphCreateLine(par, start, len);
        auto count = SBLineGetRunCount(line);
        const SBRun* runs = SBLineGetRunsPtr(line);

        for (int i = 0; i < count; i++) {
            const SBRun& run = runs[i];
            m_runs.push_back({(text_index_t)run.offset, (text_index_t)run.length, direction(run.level), {}});
            printf("run: %d, [%d, %d)\n", i, run.offset, run.offset + run.length);
        }

        start = SBParagraphGetOffset(par) + len;

        SBLineRelease(line);
        SBParagraphRelease(par);
    }

    SBAlgorithmRelease(alg);
}

void TextBlob::findBreaks() {
    auto size = m_size;

    m_wordbreaks.resize(size);
    m_graphemebreaks.resize(size);

    set_wordbreaks_utf32(m_codepoints.data(), m_codepoints.size(), nullptr, m_wordbreaks.data());
    set_graphemebreaks_utf32(m_codepoints.data(), m_codepoints.size(), nullptr, m_graphemebreaks.data());

    //    GRAPHEMEBREAK_BREAK
}

void TextBlob::shape(const TextFont& font){
    hb_buffer_t* buffer = hb_buffer_create();

    m_infos.resize(m_codepoints.size());

    for (TextRun& run : m_runs) {
        printf("shape run\n");

        auto& bounds = run.bounds;
        Point position{0, 0};
        int minX = 0;
        int maxX = 0;
        int minY = 0;
        int maxY = 0;
        hb_buffer_clear_contents(buffer);
        hb_buffer_set_direction(buffer, (hb_direction_t)run.direction);
        hb_buffer_add_codepoints(buffer, m_codepoints.data(), m_codepoints.size(), run.offset, run.length);
        hb_shape(font.font, buffer, nullptr, 0);

        const auto offset = run.offset;
        {
            unsigned int length = 0;
            auto ptr = hb_buffer_get_glyph_infos(buffer, &length);
            for (int i = 0; i < length; i++) {
                const auto& p = ptr[i];
                printf("info[%d]: %d -> %d \n", i, m_codepoints[i], ptr[i].codepoint);
                m_infos[i + offset].codepoint = p.codepoint;
                m_infos[i + offset].cluster = p.cluster;
            }
        }

        {
            unsigned int length = 0;
            auto ptr = hb_buffer_get_glyph_positions(buffer, &length);
            for (int i = 0; i < length; i++) {
                const auto& p = ptr[i];

                printf("x_advance: %d x %d\n", ptr[i].x_advance, ptr[i].y_advance);
                m_infos[i + offset].advance = {p.x_advance, p.y_advance};
                m_infos[i + offset].offset = {p.x_offset, p.y_offset};

                position.x += p.x_advance;
                position.y += p.y_advance;

                if (position.x > maxX) {
                    maxX = position.x;
                } else if (position.x < minX) {
                    minX = position.x;
                }

                if (position.y > maxY) {
                    maxY = position.y;
                } else if (position.y < minY) {
                    minY = position.y;
                }
            }

            bounds.position.x = minX;
            bounds.position.y = minY;

            bounds.size.width = maxX - minX;
            bounds.size.height = maxY - minY;
        }
    }
    hb_buffer_destroy(buffer);
};

char* printable(char c[5], int l) {
    if (l == 0) {
        c[0] = '\\';
        c[1] = '0';
        c[2] = '\0';
    } else if (l == 1) {
        char& ch = c[0];
        switch (ch) {
        case '\0':
            c[0] = '\\';
            c[1] = '0';
            c[2] = '\0';
            break;
        case '\n':
            c[0] = '\\';
            c[1] = 'n';
            c[2] = '\0';
            break;
        case ' ':
            c[0] = '\\';
            c[1] = 'w';
            c[2] = '\0';
            break;
        case '\t':
            c[0] = '\\';
            c[1] = 't';
            c[2] = '\0';
            break;
        };
    }

    return c + 2;
}

void TextBlob::debug() const {
    using json = nlohmann::json;

    json out;
    out["run"] = std::vector<json>();
    {
        // info.size() > 15 ? info.size() - 15 : 0;
        char u8[5] = {0};
        char txt[256] = {0};

        int i = 0;

        for (const TextRun& run : m_runs) {
            json r;
            r["id"] = i++;
            {
                utf8::utf32to8(m_codepoints.data() + run.offset, m_codepoints.data() + run.offset + run.length, txt)[0] = 0;
                r["text"] = txt;
            }
            { 
                json b;
                b["x"] = run.bounds.position.x;
                b["y"] = run.bounds.position.y;
                b["width"] = run.bounds.size.width;
                b["height"] = run.bounds.size.height;
                r["bounds"] = b;
            }
            r["offset"] = run.offset;
            r["length"] = run.length;

            out["run"].push_back(r);

            for (int i = run.offset; i < run.offset + run.length; i++) {
                auto r = utf8::utf32to8(m_codepoints.data() + i, m_codepoints.data() + i + 1, u8);
                r[0] = '\0';
                printable(u8, r - u8);
            }         
        }
    }

    out["infos"] = m_infos;

    printf("%s\n", out.dump(4).data());
}
