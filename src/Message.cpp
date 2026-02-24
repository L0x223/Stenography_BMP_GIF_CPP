//
// Created by user on 02/05/2025.
//

#include"Message.h"

Message::Message() : message("") {}

Message::Message(std::string oh) {

   auto message = std::string() ;
    bool inSpec = false;
    for (auto &c : oh) {
        if (c == '\"') {
            inSpec = !inSpec;
        }
        if (inSpec || c == '\"') {
            message += c;
        }
    }
    if (message[0] == '\"' && message[message.size() - 1] == '\"') {
        this->message = message.substr(1, message.size() - 2);
    }
    else
    this->message = oh;
}

auto Message::setMessage(std::string message) -> void {
    this->message = message;
}

auto Message::getMessage() -> std::string {
    return message;
}

auto Message::encryptSize() -> int {
    return this->message.size() + signature.size() + 1;
}

Message::~Message() = default;

auto Message::removeSignature(Message& message) -> void {
    std::string str = message.getMessage();
    std::string newStr = "";
    for (int i = message.signature.size(); i < str.size(); i++) {
        newStr += str[i];
    }
     message.setMessage(newStr);
}

auto Message::getBinarySize() -> const int {
    if (message != "")
        return message.size() + signature.size() + 1;
    return 0;
}

auto Message::getMessageBinary() -> uint8_t* {
    int size = message.size() + signature.size() + 1;
    auto iter = int(signature.size());

    uint8_t* binary = new uint8_t[size];

    for (int i = 0; i < signature.size(); i++) {
        binary[i] = signature[i];
    }

    for (auto ch : getMessage()) {
        binary[iter++] = ch;
    }
    binary[iter] = endOfMessage;
    return binary;
}

