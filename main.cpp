#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <utf8.h>

#include <wordbreak.h>
#include <linebreak.h>
#include <graphemebreak.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include "escape.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#pragma comment(lib, "psapi.lib")
#include <bx/bx.h>
#include <bx/hash.h>
#include <bx/string.h>

extern "C" {
#include <SheenBidi.h>
}

using Pixel = int32_t;
using Image = std::vector<Pixel>;

void copy(Image& dst, int dstW, int dstH, unsigned char* data, int x, int y, int w, int h, int s){
    if(x > dstW || y > dstH || x + w < 0 || y + h < 0){
        return;
    }
    if(x < 0){
        w += x;
        data -= x;
        x = 0;
    }
    if(y < 0){
        h += y;
        data -= y * s;
        y = 0;
    }
    if(x + w > dstW){
        w = dstW - x;
    }
    if(y + h > dstH){
        h = dstH - y;
    }
    
    Pixel* ptr = dst.data() + x + y * dstW;
    for(int j = 0; j < h; j++){
        for(int i = 0; i < w; i++){
            int id = i + j * dstW;
            int is = i + j * s;
            int px = data[is];
            if (px > 0) {
                ptr[id] = 0xffffffff;
            }
        }
    }
};

void fillRect(Image& dst, int dstW, int dstH, int x, int y, int w, int h) {
    if (x > dstW || y > dstH || x + w < 0 || y + h < 0) {
        return;
    }
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > dstW) {
        w = dstW - x;
    }
    if (y + h > dstH) {
        h = dstH - y;
    }

    Pixel* ptr = dst.data() + x + y * dstW;
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int id = i + j * dstW;
            ptr[id] = 0xff0000ff;
        }
    }
}

void strokeRect(Image& dst, int dstW, int dstH, int x, int y, int w, int h) {
    fillRect(dst, dstW, dstH, x, y, w, 1);
    fillRect(dst, dstW, dstH, x, y + h - 1, w, 1);

    fillRect(dst, dstW, dstH, x, y, 1, h);
    fillRect(dst, dstW, dstH, x + w - 1, y, 1, h);
}

void writeImage(const std::string& path, int width, int height, const Image& data){
    stbi_write_png(path.c_str(), width, height, 4, data.data(), width * 4);
}

std::string readFile(const std::string& path){
    std::ifstream ifs(path, std::ios_base::binary);
    return std::string( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
}

int onFtError(FT_Error error_code){
    const char* msg = FT_Error_String(error_code);
    std::cout << "FT Error: " << (msg ? msg : "<?>") << "\n";
    return error_code;
}

template<class ElementType>
class span{
public:
    using element_type = ElementType;
    using value_type = std::remove_cv_t<ElementType>;
    using index_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = element_type*;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using const_reference = const element_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    static const size_t npos = -1;
    
    constexpr span() noexcept : span(nullptr, (index_type)0) {}
    constexpr span(pointer ptr, index_type count) noexcept
    : ptr(ptr), count(count) {}
    constexpr span(pointer first, pointer last) noexcept
    : ptr(ptr), count(last - first) {}
    constexpr span(const span& other) noexcept = default;
    ~span() noexcept = default;
    
    constexpr span& operator=(const span& other) noexcept = default;
    
    constexpr reference operator[](index_type idx) const { return ptr[idx]; };
    
    constexpr index_type size() const noexcept { return count; };
    
private:
    pointer ptr;
    index_type count;
};


#include "text.hpp"

int main(int argc, char** args){
    const std::string fileData = readFile("../data/OTF/SourceSansPro-Regular.otf");
    //const std::string fileData = readFile("../data/TTF/Scheherazade-Regular.ttf");
    
    FT_Library  library;
    auto error = FT_Init_FreeType( &library );
    if ( error )
    {
        return onFtError(error);
    }
    
    FT_Face face;
    error = FT_New_Memory_Face(library, (const FT_Byte*)fileData.data(), fileData.size(), 0, &face);
    
    if ( error )
    {
        return onFtError(error);
    }
    
    error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    
    hb_face_t* hbFace = hb_ft_face_create_referenced(face);
    hb_font_t* hbFont = hb_font_create(hbFace);

    TextFont font{hbFont};

    const std::string input = "Hello world!\nThe word <often> often has ligatures\nBye!\n"; // readFile("../data/utf-8/short-bidi.txt");
    const hb_direction_t inputDirection = HB_DIRECTION_LTR;
    const std::string inputLanguage = "en";

    TextBlob blob;
    blob.set(input);
    blob.findBreaks();
    blob.shape(font);
    //blob.debug();

    //blob.visit();

    {
        int fontSize = 20;
        const int width = 512;
        const int height = 512;
        Image img;
        img.resize(width * height * sizeof(Pixel), 0xff000000);
        FT_Set_Pixel_Sizes(face, fontSize, fontSize);

        int x0 = 1000;
        int y0 = 1000 * 2;

        int x = x0;
        int y = y0;


        strokeRect(img, width, height, 10, 20, 30, 40);
        for (const TextRun& run : blob.m_runs) {

            //int maxWidth = 1500;
            //blob.m_infos[run.offset].
            //while (w < maxWidth) {
            //}



            for (int i = 0; i < run.length; i++) {
                auto index = run.offset + i;
                auto gp = blob.m_infos[index];

                if (gp.codepoint) {
                    error = FT_Load_Glyph(face, gp.codepoint, FT_LOAD_RENDER);
                    if (error) {
                        return onFtError(error);
                    }   

                    FT_GlyphSlot g = face->glyph;
                    FT_Bitmap b = face->glyph->bitmap;
                    int xx = (x + 0*gp.advance.x) / 1000.0 * fontSize;
                    int yy = (y + 0*gp.advance.y) / 1000.0 * fontSize;
                    copy(img, width, height, b.buffer, xx + g->bitmap_left, yy - g->bitmap_top, b.width, b.rows, b.pitch);
                }

                x += gp.advance.x;
                y += gp.advance.y;
            }

            y += 1.25 * 1000;
            x = x0;

            std::cout << x << "," << y << std::endl;
        }
        
        writeImage("../../out.png", width, height, img);
    }






//    TextShaper shaper;
//    shaper.shape(blob);



    return 0;

    auto size = utf8::distance(input.begin(), input.end());
    std::vector<uint32_t> u32;
    u32.resize(size);
    utf8::utf8to32(input.begin(), input.end(), u32.data());
    std::vector<char> brks;
    brks.resize(size);

    auto hash = bx::hash<bx::HashMurmur2A>((char*)u32.data(), (size_t)u32.size() * 4);
    



    hb_buffer_t* buffer = hb_buffer_create();
    //hb_buffer_set_language(buffer, hb_language_from_string(inputLanguage.data(), inputLanguage.size()) );
    hb_buffer_set_direction(buffer, inputDirection);
    hb_buffer_add_utf32(buffer, u32.data(), size, 0, size);
    {
        SBCodepointSequence codepointSequence = {SBStringEncodingUTF32, (void*)u32.data(), u32.size()};

        /* Extract the first bidirectional paragraph. */
        SBAlgorithmRef bidiAlgorithm = SBAlgorithmCreate(&codepointSequence);
        SBParagraphRef firstParagraph = SBAlgorithmCreateParagraph(bidiAlgorithm, 0, INT32_MAX, SBLevelDefaultRTL);
        SBUInteger paragraphLength = SBParagraphGetLength(firstParagraph);

                    printf("Run Level: %s\n\n", SBParagraphGetBaseLevel(firstParagraph) % 2 ? "RTL" : "LTR");
        hb_direction_t dir = SBParagraphGetBaseLevel(firstParagraph) % 2 ? HB_DIRECTION_RTL : HB_DIRECTION_LTR;
        //hb_buffer_set_direction(buffer, dir);


        /* Create a line consisting of whole paragraph and get its runs. */
        SBLineRef paragraphLine = SBParagraphCreateLine(firstParagraph, 0, paragraphLength);
        SBUInteger runCount = SBLineGetRunCount(paragraphLine);
        const SBRun* runArray = SBLineGetRunsPtr(paragraphLine);
        auto l = SBParagraphGetBaseLevel(firstParagraph);
        /* Log the details of each run in the line. */
        for (SBUInteger i = 0; i < runCount; i++) {
            printf("Run Offset: %ld\n", (long)runArray[i].offset);
            printf("Run Length: %ld\n", (long)runArray[i].length);
            printf("Run Level: %s\n\n", runArray[i].level % 2 ? "RTL" : "LTR");

            hb_direction_t dir = runArray[i].level % 2 ? HB_DIRECTION_RTL : HB_DIRECTION_LTR;
            //hb_buffer_set_segment_properties(buffer, props);

        }

        /* Create a mirror locator and load the line in it. */
        SBMirrorLocatorRef mirrorLocator = SBMirrorLocatorCreate();
        SBMirrorLocatorLoadLine(mirrorLocator, paragraphLine, (void*)u32.data());
        const SBMirrorAgent* mirrorAgent = SBMirrorLocatorGetAgent(mirrorLocator);

        /* Log the details of each mirror in the line. */
        while (SBMirrorLocatorMoveNext(mirrorLocator)) {
            printf("Mirror Index: %ld\n", (long)mirrorAgent->index);
            printf("Actual Code Point: %ld\n", (long)mirrorAgent->codepoint);
            printf("Mirrored Code Point: %ld\n\n", (long)mirrorAgent->mirror);
        }

        /* Release all objects. */
        SBMirrorLocatorRelease(mirrorLocator);
        SBLineRelease(paragraphLine);
        SBParagraphRelease(firstParagraph);
        SBAlgorithmRelease(bidiAlgorithm);
    }
    hb_shape(hbFont, buffer, nullptr, 0);
    
    span<hb_glyph_info_t> info;
    span<hb_glyph_position_t> pos;
    
    {
        unsigned int length = 0;
        auto ptr = hb_buffer_get_glyph_infos(buffer, &length);
        
        info = {ptr, length};
    }
    
    {
        unsigned int length = 0;
        auto ptr = hb_buffer_get_glyph_positions(buffer, &length);
        
        pos = {ptr, length};
    }
    
    { 
        set_linebreaks_utf32(u32.data(), u32.size(), inputLanguage.data(), brks.data());
    }

    hb_position_t length = 0;
    for(int i = 0; i<pos.size(); i++){
        length += pos[i].x_advance;
    }
    std::cout << "length: " << length << "\n";
    std::cout << "" << u32.size() << "\n";
    
    {
        int i = 0;
        //info.size() > 15 ? info.size() - 15 : 0;
        char u8[5] = {1,0,3,4,0};
        printf("|%5s|%5s|%5s|%5s|%5s|%5s|\n", "id", "u32", "xadv", "cp", "cl", "u8");
        printf("|-----|-----|-----|-----|-----|-----|\n");
        for(; i<info.size(); i++){
            auto r = utf8::utf32to8(u32.data() + i, u32.data() + i + 1, u8);
            r[0] = '\0';
            printf("|%5d|%5d|%5d|%5d|%5d|%5s| %s\n", i, u32[i], pos[i].x_advance, info[i].codepoint, info[i].cluster ,u8, escape(u32[i]).c_str());
        }
    }
    
    
    
    
    
    
    {
        int fontSize = 20;
        const int width = 512;
        const int height = 512;
        Image img;
        img.resize(width * height * sizeof(Pixel));
        FT_Set_Pixel_Sizes(face, fontSize, fontSize);

        int x = 1000;
        int y = 1000 * 2;
        for(int i = 0; i<info.size(); i++){
            auto gi = info[i];
            auto gp = pos[i];
            
            if (brks[i] <= LINEBREAK_ALLOWBREAK) {
                x = 1000;
                y += 1.25 * 1000;
                continue;
            }
            
            if(!gi.codepoint){
                continue;
            }
            
            error = FT_Load_Glyph(face, info[i].codepoint, FT_LOAD_RENDER);
            if ( error )
            {
                return onFtError(error);
            }
            
            FT_GlyphSlot g = face->glyph;
            FT_Bitmap b = face->glyph->bitmap;
            int xx = (x + gp.x_offset) / 1000.0 * fontSize;
            int yy = (y + gp.y_offset) / 1000.0 * fontSize;
            copy(img, width, height, b.buffer, xx + g->bitmap_left, yy - g->bitmap_top, b.width, b.rows, b.pitch);
            
            x += gp.x_advance;
            y += gp.y_advance;
            //img.resize(b.width * b.rows);
            //img.assign(b.buffer, b.buffer + b.width * b.rows);
            /*for(int i = 0; i<img.size(); i++){
             img[i] = (char)(255.0*(float)i/img.size());
             }*/
        }
        writeImage("../../out.png", width, height, img);
    }
    
    
    
    return argc;
}
