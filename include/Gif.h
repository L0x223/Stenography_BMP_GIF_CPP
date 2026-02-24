#pragma once

#include <map>
#include <string>
#include "Message.h"
#include<vector>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <cmath>

namespace gf {
    enum EXTENSIONS {
        ApplicationExtensionLabel = 0xFF,
        CommentLabel = 0xFE,
        GraphicsControlExtension = 0xF9,
        PlainTextExtension = 0x01
    };
}

class Gif {

    int width, height;

    static const int terminator_size = 1;
   static const int introduserSize = 1;
   static const int headerSize = 6;
   static const int FIRST_packedField_pos = 10;
   static const int globalColorTable_pos = 13;
   static const int extensionIntroducer = 21;
   static const int Labeles_first2bytes = 2;
   static const int application_Extension_Label_size = 11;
    static const int packedField = 8;

    static const std::string format;

    static const uint8_t trailer = 0x3B;

    bool GCT_flag = false;



     int GCT_size = 0;
    std::vector<bool> LCT_flagVector;
   std::map<int, std::pair<int, int>> colorTable_pair_pos_size;
    std::string path;

    Message m_message;


     auto setInfo() -> void;
    auto isIntroduser(std::fstream&) -> bool;

    auto isReadable(std::fstream&) -> bool;
    auto extentionSize(int) -> int;
    auto isLastbitSame(uint8_t const&, uint8_t const&) -> bool;
    auto skipExtension(int, std::fstream&) -> void;

    public:

    Gif(std::string path);

    static auto resolution (std::string) -> std::string;
    auto setMessage(Message) -> void;
    auto getMessage() -> Message;
    auto setPath(std::string) -> void;
    auto isValid( Message) -> bool;
    static auto info(std::string) -> void;
    static auto isGif(std::string) -> bool;
    ~Gif();
};
