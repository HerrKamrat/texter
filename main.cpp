#include <fstream>
#include <iostream>
#include <iomanip>
#include <span>
#include <sstream>
#include <string>
#include <vector>

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

void copy(std::vector<char>& dst, int dstW, int dstH, unsigned char* data, int x, int y, int w, int h, int s){
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
    
    char* ptr = dst.data() + x + y * dstW;
    for(int j = 0; j < h; j++){
        for(int i = 0; i < w; i++){
            int id = i + j * dstW;
            int is = i + j * s;
            int px = ptr[id] + data[is];
            ptr[id] = px > 255 ? 255 : px;
        }
    }
};


void writeImage(const std::string& path, int width, int height, const std::vector<char>& data){
    stbi_write_png(path.c_str(), width, height, 1, data.data(), width);
}

std::string readFile(const std::string& path){
    std::ifstream ifs(path);
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

struct Text {
    std::vector<uint32_t> u32;
};

int main(int argc, char** args){
    const std::string fileData = readFile("../../data/OTF/SourceSansPro-Regular.otf");
    const std::string input = readFile("../../data/utf-8/hello.txt");
    const hb_direction_t inputDirection = HB_DIRECTION_LTR;
    const std::string inputLanguage = "en";
    
    
    auto size = utf8::distance(input.begin(), input.end());
    std::vector<uint32_t> u32;
    u32.resize(size);
    utf8::utf8to32(input.begin(), input.end(), u32.data());
    
    
    
    
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
    auto hbFont = hb_font_create(hbFace);
    
    
    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_set_language(buffer, hb_language_from_string(inputLanguage.data(), inputLanguage.size()) );
    
    hb_buffer_add_utf32(buffer, u32.data(), size, 0, size);
    hb_buffer_set_direction(buffer, inputDirection);
    
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
    
    hb_position_t length = 0;
    for(int i = 0; i<pos.size(); i++){
        length += pos[i].x_advance;
    }
    std::cout << "length: " << length << "\n";
    std::cout << "" << u32.size() << "\n";
    
    {
        int i = info.size() > 15 ? info.size() - 15 : 0;
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
        const int width = 512;
        const int height = 512;
        std::vector<char> img;
        img.resize(width * height);
        FT_Set_Pixel_Sizes(face, 20, 20);

        int x = 1000;
        int y = 1000 * 2;
        for(int i = 0; i<info.size(); i++){
            auto gi = info[i];
            auto gp = pos[i];
            
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
            int xx = (x + gp.x_offset) / 1000.0 * 20;
            int yy = (y + gp.y_offset) / 1000.0 * 20;
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
