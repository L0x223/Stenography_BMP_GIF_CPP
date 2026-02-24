
#include <fstream>
#include <cmath>
#include "Bmp.h"

#include <iostream>


#include "Message.h"

std::string Bmp::format = "BM";

Bmp::Bmp() {}

Bmp::Bmp(const std::string path) : path(path) {
    auto file = std::ifstream(path);
    file.seekg(0, std::ios::beg);
    if (!file.is_open()) {
        std::cerr << "Could not open file:  " << path << std::endl;
        return;
    }
    file.seekg(18);
    file.read((char*)&this->width, 4);
    file.read((char*)&this->height, 4);
    file.seekg(36);
    file.close();
}

auto Bmp::info(std::string path) -> void {
    if (Bmp::isBmp(path)) {
        std::string resolution;
        std::filesystem::path fsPath(path);
        const auto fileTime = std::filesystem::last_write_time(fsPath);
        const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileTime);
        const auto time = std::chrono::system_clock::to_time_t(systemTime);

        std::cout << "Format: " << format << std::endl << "Resolution: " << Bmp::resolution(path) << std::endl
        << "File size: " << std::filesystem::file_size(path)<< std::endl << "Last data of modify time: " <<
            std::put_time(std::localtime(&time), "%d-%m-%Y %H:%M:%S")<< std::endl;
    }
}

auto Bmp::resolution(std::string path) -> std::string {
    auto file = std::ifstream(path, std::ios::binary | std::ios::in);
    file.seekg(0, std::ios::beg);
    file.seekg(18);
    int width, height;
    file.read(reinterpret_cast<char*>(&width), 4);
    file.read(reinterpret_cast<char*>(&height), 4);
    return std::to_string(width)+"x"+std::to_string(height);
}

auto Bmp::isLastbitSame(uint8_t const & col, uint8_t const & bit) -> bool {
    return col%2 == bit%2;
}

auto Bmp::removeSignatures(std::string const& str) -> std::string {
    auto newStr = std::string();
    for (int i = m_message.signature.size(); i < str.size(); i++) {
        newStr += str[i];
    }
    return newStr;
};

auto Bmp::enoughSpace(Message& mes) -> bool {
    const int pudding = ((4 - (this->width*3)%4)%4);
    int size = mes.getBinarySize();
    int photo_size = height*(width - pudding);
    return photo_size >= size;
}

auto Bmp::isBmp(std::string path) -> bool {
    auto file = std::fstream(path, std::ios::binary | std::ios::in);

    char form[format.size()];
    file.seekg(0, std::ios::beg);
    file.read(form, format.size() | std::ios::beg);
    for (int i = 0; i < format.size(); i++) {
        if (form[i] != format[i]) {
            file.close();
            return false;
        }
    }
    file.close();
    return true;
}

auto Bmp::isBMP(std::fstream& file) -> bool {

    auto possition = file.tellg();

    char form[format.size()];
    file.seekg(0, std::ios::beg);
    file.read(form, format.size() | std::ios::beg);
    for (int i = 0; i < format.size(); i++) {
        if (form[i] != format[i]) {
            file.close();
            return false;
        }
    }
    file.seekg(possition, std::ios::beg);
    return true;
}

auto Bmp::isReadable(std::fstream& file) -> bool {

    auto possition = file.tellg();

    int size = m_message.signature.size();
    auto newStr = std::string();
    auto i = int();
    int pudCount = 0;
    const int pudding = ((4 - (this->width*3)%4)%4);
    auto mesChar = uint8_t();
    auto bit = uint8_t();
    auto counter = 0;;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width*3; x++) {
            if (counter == 8) {
                if (i++ >= size) {
                    break;
                }
                counter = 0;
                newStr += mesChar;
                mesChar = 0;
            }
            file.read(reinterpret_cast<char*>(&bit), 1);
            mesChar += std::pow(2, counter++)*(bit & 1);
        }
        if (i >= size)
        {break;}
        file.seekg(pudding, std::ios::cur);
        pudCount++;
    }

    file.seekg(possition);
    if (newStr == m_message.signature) {
        return true;
    }
    file.close();
    return false;
}


auto Bmp::SetPath(std::string path) -> void  {
    this->path = path;
    auto file = std::ifstream(path);
    file.seekg(0, std::ios::beg);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }
    file.seekg(18);
    file.read((char*)&this->width, 4);
    file.read((char*)&this->height, 4);
    file.seekg(36);
    file.close();
}

auto Bmp::getMessage() -> const Message {
    auto file = std::fstream(path,std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return Message();
    }

    if (!isBMP(file)) {
        std::cerr << "file is not BMP: " << path << std::endl;
        return Message();
    }

    file.seekg(fileInfo,std::ios::beg);

    if (!isReadable(file)) {
        std::cerr << "File does not contain a message: " << path << std::endl;
        return Message();
    }

    const int pudding = ((4 - (this->width*3)%4)%4);
    auto message = std::string("");
    auto mesChar = uint8_t();
    auto bit = uint8_t();
    auto counter = 0;
    auto end = bool(false);
    for (int y = 0; y < this->height; y++) {
        if (end){break;}
        for (int x = 0; x < this->width*3; x++) {
            if (counter == 8) {
                counter = 0;
                message += (char)mesChar;
                if (mesChar == 0) {
                    end = true;
                    break;
                }
                mesChar = 0;
            }
            file.read(reinterpret_cast<char*>(&bit), 1);
            mesChar += std::pow(2, counter++)*(bit & 1);
        }
        file.seekg(pudding, std::ios::cur);
    }
    file.close();

    auto endMessage = Message(removeSignatures(message));
    return endMessage;
}


auto Bmp::setMessage(Message message) -> void {
    auto file = std::fstream(path,std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }
    auto bit = uint8_t();
    const int pudding = ((4 - (this->width*3)%4)%4);
    int counter = 0;
    int byter = 0;

    file.seekg(fileInfo, std::ios::beg);

    uint8_t* secretMas = message.getMessageBinary();

    if (!isBMP(file)) {
        std::cerr << "file is not BMP: " << path << std::endl;
    }

    if (!enoughSpace(message)) {
        std::cerr << "Bmp file hasn't enough Space: " << path << std::endl;
        file.close();
        return;
    }

    bool end = false;
    for (int y = 0; y < this->height; y++) {
        if (end) {break;}
        for (int x = 0; x < this->width*3; x++) {
            if (counter == 8) {
                if (byter++ > message.encryptSize()){
                    end = true;
                    break;
                }
                counter = 0;
            }
            file.read(reinterpret_cast<char*>(&bit), 1);
            file.seekp(-1, std::ios::cur);
            if (!this->isLastbitSame(bit, (secretMas[byter] >> counter))){
                if (bit % 2)
                    bit &= ~1;
                else bit |= 1;
            }
            file.write(reinterpret_cast<char*>(&bit), 1);

            counter++;
        }
        file.seekg(pudding, std::ios::cur);
    }
    delete[] secretMas;

    file.close();

}

Bmp::~Bmp() = default;

