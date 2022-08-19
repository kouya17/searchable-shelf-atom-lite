#pragma once

#include <Arduino.h>
#include <string>

namespace util {
namespace string {

std::string getMultiByteAlfa(unsigned char s) {
    switch (s)
    {
    case 0x41: return "Ａ";
    case 0x42: return "Ｂ";
    case 0x43: return "Ｃ";
    case 0x44: return "Ｄ";
    case 0x45: return "Ｅ";
    case 0x46: return "Ｆ";
    case 0x47: return "Ｇ";
    case 0x48: return "Ｈ";
    case 0x49: return "Ｉ";
    case 0x4A: return "Ｊ";
    case 0x4B: return "Ｋ";
    case 0x4C: return "Ｌ";
    case 0x4D: return "Ｍ";
    case 0x4E: return "Ｎ";
    case 0x4F: return "Ｏ";
    case 0x50: return "Ｐ";
    case 0x51: return "Ｑ";
    case 0x52: return "Ｒ";
    case 0x53: return "Ｓ";
    case 0x54: return "Ｔ";
    case 0x55: return "Ｕ";
    case 0x56: return "Ｖ";
    case 0x57: return "Ｗ";
    case 0x58: return "Ｘ";
    case 0x59: return "Ｙ";
    case 0x5A: return "Ｚ";
    case 0x61: return "ａ";
    case 0x62: return "ｂ";
    case 0x63: return "ｃ";
    case 0x64: return "ｄ";
    case 0x65: return "ｅ";
    case 0x66: return "ｆ";
    case 0x67: return "ｇ";
    case 0x68: return "ｈ";
    case 0x69: return "ｉ";
    case 0x6A: return "ｊ";
    case 0x6B: return "ｋ";
    case 0x6C: return "ｌ";
    case 0x6D: return "ｍ";
    case 0x6E: return "ｎ";
    case 0x6F: return "ｏ";
    case 0x70: return "ｐ";
    case 0x71: return "ｑ";
    case 0x72: return "ｒ";
    case 0x73: return "ｓ";
    case 0x74: return "ｔ";
    case 0x75: return "ｕ";
    case 0x76: return "ｖ";
    case 0x77: return "ｗ";
    case 0x78: return "ｘ";
    case 0x79: return "ｙ";
    case 0x7A: return "ｚ";
    default: break;
    }
    return "";
}

std::string toMultiByte(std::string s) {
    std::string ret = "";
    int char_size = 0;
    unsigned char lead = 0;
    for (int pos = 0; pos < s.size(); pos += char_size) {
   
        lead = s[pos];

        if (lead < 0x80) {
            char_size = 1;
            if ((lead >= 0x41 && lead <= 0x5A) || lead >= 0x61 && lead <= 0x7A) {
                ret += getMultiByteAlfa(lead);
                continue;
            }
        } else if (lead < 0xE0) {
            char_size = 2;
        } else if (lead < 0xF0) {
            char_size = 3;
        } else {
            char_size = 4;
        }

        ret += s.substr(pos, char_size);
    }
    return ret;
}

}
}