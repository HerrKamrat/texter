//
//  escape.cpp
//  texter
//
//  Created by Gabriel on 2019-07-03.
//

#include "escape.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <span>
#include <sstream>
#include <string>
#include <vector>

#include <utf8.h>


std::string escape(uint32_t u){
    char u8[4] = {0};
    uint16_t u16[2] = {0};
    
    auto r1 = utf8::utf32to8(&u, &u + 1, u8);
    auto r2 = utf8::utf8to16(u8, r1, u16);
    
    std::stringstream ss;
    ss << std::hex;
    for(auto r = u16; r < r2 && r[0]; r++){
        ss << "\\u" << std::setfill('0') << std::setw(4) << r[0];
    }
    
    return ss.str();
}
