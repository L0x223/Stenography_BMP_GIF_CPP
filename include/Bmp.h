#pragma once
#include <string>
#include <filesystem>
#include <chrono>
#include "Message.h"

class Bmp {

    Message m_message;
    const int fileInfo = 54;
    std::string path;
    int width = 0;
    int height = 0;
    static std::string format;


    static auto resolution(std::string) -> std::string;
    auto isLastbitSame(uint8_t const&, uint8_t const&) -> bool;
    auto removeSignatures(std::string const&) -> std::string;
    auto isReadable(std::fstream&) -> bool;


    auto isBMP(std::fstream&) -> bool;
    public:
        Bmp();
        Bmp(std::string);

        auto Import_data() -> void;
        auto Copy_data(std::string) -> void;

        auto enoughSpace(Message&) -> bool;
        auto SetPath(std::string) -> void;
        static auto info(std::string) -> void;
        static auto isBmp(std::string) -> bool;
        auto setMessage(Message Message) -> void;
        auto getMessage() -> const Message;

        ~Bmp();

};
