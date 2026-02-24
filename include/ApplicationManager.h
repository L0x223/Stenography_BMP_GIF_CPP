#pragma once
#include "Bmp.h"
#include "Gif.h"

enum FLAG {
INFO, ENCRYPT, DECRYPT, CHECK, HELP
};

class ApplicationManager {

    std::map<int, std::pair<FLAG, std::string>> applicationMap;

    auto info(const std::string&) -> void;
    auto encrypt(const std::string&) -> void;
    auto decrypt(const std::string&) -> void;
    auto check(const std::string&) -> void;
    auto help() -> void;


    auto applicationSlicer(const std::string& ) -> std::vector<std::string>;
    auto analyze(const  std::vector<std::string>&) -> int;
    auto applicationMagic() -> void;
    public:
    ApplicationManager();
    auto start() -> void;
    ~ApplicationManager() = default;
};

