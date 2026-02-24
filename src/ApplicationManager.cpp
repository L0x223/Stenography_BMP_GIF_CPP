#include "ApplicationManager.h"


auto ApplicationManager::info(const std::string & path) -> void {
    auto const fsPath = std::filesystem::path(path);
    if (std::filesystem::exists(path)) {
        if (Bmp::isBmp(path)) {
            Bmp::info(path);
        }else if (Gif::isGif(path)) {
            Gif::info(path);
        }
        else {
        std::cout <<"We do not support this file: " << path << std::endl;
        }
    }
    else {
        std::cerr << "File not found: " << path << std::endl;
    }
}



auto ApplicationManager::encrypt(const std::string &line) -> void {
    auto path = std::string();
    int pos = 0;
    for (const auto &c : line) {
        if (c == '\"') {
            break;
        }
        pos++;
        path += c;
    }
    auto message = line.substr(pos);
    auto const fsPath = std::filesystem::path(path);
    Message myMessage = Message(message);
    if (std::filesystem::exists(path)) {
        if (Bmp::isBmp(path)) {
            Bmp bmp(path);
            bmp.setMessage(myMessage);
        }else if (Gif::isGif(path)) {
            Gif gif(path);
            gif.setMessage(myMessage);
        }
        else {
            std::cout <<"We do not support this file: " << path << std::endl;
        }
    }
    else {
        std::cerr << "File not found: " << path << std::endl;
    }
}

auto ApplicationManager::decrypt(const std::string &path) -> void {
    auto const fsPath = std::filesystem::path(path);
    auto receive = Message();
    if (std::filesystem::exists(path)) {
        if (Bmp::isBmp(path)) {
            Bmp bmp(path);
            receive = bmp.getMessage();
            std::cout <<receive.getMessage() << std::endl;
        }else if (Gif::isGif(path)) {
            Gif gif(path);
            receive = gif.getMessage();
            std::cout <<receive.getMessage() << std::endl;
        }
        else {
            std::cout <<"We do not support this file: " << path << std::endl;
        }
    }
    else {
        std::cerr << "File not found: " << path << std::endl;
    }
}

auto ApplicationManager::check(const std::string & line) -> void {
    auto path = std::string();
    int pos = 0;
    for (const auto &c : line) {
        if (c == '\"') {
            break;
        }
        pos++;
        path += c;
    }
    auto message = line.substr(pos);
    auto const fsPath = std::filesystem::path(path);
    auto receive = Message(message);

    bool check = false;
    if (std::filesystem::exists(path)) {
        if (Bmp::isBmp(path)) {
            Bmp bmp(path);
            if (bmp.enoughSpace(receive)) {
               check = true;
            }
        }
        else if (Gif::isGif(path)) {
            Gif gif(path);
            if (gif.isValid(receive)) {
                check = true;
            }
        }
        else {
            std::cout <<"We do not support this file: " << path << std::endl;
        }
    }
    else {
        std::cerr << "File not found: " << path << std::endl;
    }
    if (check) {
        std::cout <<"Is enough space: " <<path << std::endl;
    }
    else {
        std::cout <<"Is not enough space: " <<path << std::endl;
    }
}

auto ApplicationManager::help() -> void {
    auto file = std::fstream("helper.txt", std::ios::in);

    if (!file.is_open()) {
        std::cerr << "Could not open the file!\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}

auto ApplicationManager::applicationSlicer(const std::string& line) -> std::vector<std::string> {

    auto slice = std::string();

    std::vector<std::string> result;

    for (char ch : line) {
        if (ch == ' ') {
            if (slice != "") {
                result.push_back(slice);
                slice = "";
            }
        }else{
            slice += ch;
        }
    }
    result.push_back(slice);
    return result;
}

auto ApplicationManager::analyze(const  std::vector<std::string> & vector) -> int {
    int key = 0;

        bool notFirst = false;

    FLAG flag;
    std::string tmp_expr;
    for (std::string expr : vector) {
        if (expr[0] == '-') {
            if (notFirst) {
                applicationMap[key++] = std::make_pair(flag, tmp_expr);
                tmp_expr = "";
            }

            if (expr == "-i" || expr == "--info") {
                flag = INFO;
            } else if (expr == "-e" || expr == "--encrypt") {
                flag = ENCRYPT;
            } else if (expr == "-d" || expr == "--decrypt") {
                flag = DECRYPT;
            } else if (expr == "-c" || expr == "--check") {
                flag = CHECK;
            } else if (expr == "-h" || expr == "--help") {
                flag = HELP;
            } else {
                std::cerr << "Unknown flag: " << expr << " , use supported flag or use --help command" <<std::endl;
                return 1;
            }

            notFirst = true;
        } else {
            tmp_expr += expr + " ";
        }
    }
    applicationMap[key] = std::make_pair(flag, tmp_expr);

    return 0;
}

auto ApplicationManager::start() -> void {
    auto line = std::string();
    std::getline(std::cin, line);

   if (analyze(applicationSlicer(line)))
       return;
    applicationMagic();
}

ApplicationManager::ApplicationManager() {

}


auto ApplicationManager::applicationMagic() -> void {
    for (const auto& [key, value] : applicationMap) {
        switch (value.first) {
            case INFO: info(value.second); break;
            case ENCRYPT: encrypt(value.second); break;
            case DECRYPT: decrypt(value.second); break;
            case CHECK: check(value.second); break;
            case HELP: help(); break;
        }
    }
}