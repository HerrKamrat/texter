#include <fstream>
#include <iostream>
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

template<typename T>
struct view{
    using value_type = T;
    using pointer_type = value_type*;

    pointer_type ptr;
    size_t count;
    
    value_type& operator[](std::size_t idx)       { return ptr[idx]; }
    const value_type& operator[](std::size_t idx) const { return ptr[idx]; }
};

struct Text {
    std::vector<uint32_t> u32;
};

int main(int argc, char** args){
    const std::string fileData = readFile("../../data/OTF/SourceSansPro-Regular.otf");
    const std::string input = readFile("../../data/utf-8/hello.txt");
    const hb_direction_t inputDirection = HB_DIRECTION_LTR;
    const std::string inputLanguage = "en";

    
    Text text;
    
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
    
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    hb_face_t* hbFace = hb_ft_face_create_referenced(face);
    auto hbFont = hb_font_create(hbFace);
    

    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_set_language(buffer, hb_language_from_string(inputLanguage.data(), inputLanguage.size()) );
    
    hb_buffer_add_utf32(buffer, u32.data(), size, 0, size);
    hb_buffer_set_direction(buffer, inputDirection);

    hb_shape(hbFont, buffer, nullptr, 0);
    
    view<hb_glyph_info_t> info = {0};
    view<hb_glyph_position_t> pos = {0};

    //unsigned int         glyph_info_count;
    //hb_glyph_info_t     *glyph_info =
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
    for(int i = 0; i<pos.count; i++){
        length += pos[i].x_advance;
    }
    std::cout << "length: " << length << "\n";
    std::cout << "" << u32.size() << "\n";
    
    {
        int i = info.count > 15 ? info.count - 15 : 0;
        char u8[5] = {1,0,3,4,0};
        printf("|%5s|%5s|%5s|%5s|%5s|%5s|\n", "id", "u32", "xadv", "cp", "cl", "u8");
        printf("|-----|-----|-----|-----|-----|-----|\n");
        for(; i<info.count; i++){
            auto r = utf8::utf32to8(u32.data() + i, u32.data() + i + 1, u8);
            r[0] = '\0';
            printf("|%5d|%5d|%5d|%5d|%5d|%5s\n", i, u32[i], pos[i].x_advance, info[i].codepoint, info[i].cluster, u8);
        }
    }
    
    {
        uint32_t d = 128542;
        uint16_t a = 55357;
        uint16_t b = 56862;
        
        std::stringstream ss;
        ss<< std::hex << a; // int decimal_value
        ss << ", " << b;
        std::string res ( ss.str() );
        
        std::cout << res << "\n";
    
    }

    
    /*
     
    char* wordbreaks = new char[size];
    set_wordbreaks_utf32(u32, size, "", wordbreaks);
    
    for(int i = 0; i < size; i++){
        std::cout << u32[i] << '|';
    }
    std::cout << '\n';
    
    for(int i = 0; i < size; i++){
        std::cout << (int)wordbreaks[i] << '|';
    }
    std::cout << '\n';
    std::cout << hb_buffer_get_length(b);
    std::cout << '\n';
    */
    return argc;
}
