#pragma once
#include <cstdint>
#include <iostream>
#include <string>

class Message {

    std::string message;


    public:

    Message();
    Message(std::string message);

    auto getBinarySize() -> const int;
    auto getMessage() -> std::string;
    auto getMessageBinary() -> uint8_t*;
    auto setMessage(std::string message) -> void;

    auto encryptSize() -> int;
    ~Message();

    const std::string signature = "MB";
    const char endOfMessage = '\0';

    static auto removeSignature(Message&) -> void;

    auto operator=(const Message& other){
        message = other.message;
    }
};
