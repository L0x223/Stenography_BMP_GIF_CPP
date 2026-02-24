#include"Gif.h"



std::string const Gif::format = "GIF";

auto Gif::isValid(Message message) -> bool {
    auto setSize = int();
    auto mesSize = message.getBinarySize();

  for (const auto& [key, value] : colorTable_pair_pos_size)
      setSize += value.second;
    setSize /= 8;
    return setSize >= mesSize;
}

auto Gif::info(std::string path) -> void {
    if (Gif::isGif(path)) {
        std::string resolution;
        std::filesystem::path fsPath(path);
        const auto fileTime = std::filesystem::last_write_time(fsPath);
        const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileTime);
        const auto time = std::chrono::system_clock::to_time_t(systemTime);
        std::cout << "Format: " << format << std::endl << "Resolution: " << Gif::resolution(path) << std::endl
        << "File size: " << std::filesystem::file_size(path)<< std::endl << "Last data of modify time: " << time << std::endl;
    }
}

 auto Gif::setInfo() -> void {
     auto file = std::fstream(path, std::ios::binary | std::ios::in);


    file.seekg(0, std::ios::end);
     int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);


       auto pos_key = 0;
     uint8_t byte;

     file.seekg(0, std::ios::beg);
     if (!file.is_open()) {
         std::cerr << "Could not open file: " << path << std::endl;
         return;
     }
     file.seekg(FIRST_packedField_pos, std::ios::beg);

     file.read(reinterpret_cast<char*>(&byte), 1);
     if (byte >> 7) {
         int unUsedBite = (byte >> 3)*std::pow(2, 3);
         GCT_flag = true;
         GCT_size = 3*std::pow(2, (byte - unUsedBite )+ 1);

     }
     file.seekg(globalColorTable_pos, std::ios::beg);
      colorTable_pair_pos_size[pos_key++] = {file.tellg(), GCT_size};
     file.seekg(GCT_size, std::ios::cur);

while (true) {

    while (isIntroduser(file)) {
        file.read(reinterpret_cast<char*>(&byte), 1);
        int eSize = extentionSize((int)byte);
        skipExtension(eSize, file);
    }

    file.seekg(-1, std::ios::cur);
    file.read(reinterpret_cast<char*>(&byte), 1);

    /////////////////////////////////////////
    if (byte == trailer) {
        file.close();
        return;
    }
    /////////////////////////////////////////

    int tmpLocalColorTableSize = 0;
    if ((int)byte == 0x2C) {
        file.seekg(packedField, std::ios::cur);
        file.read(reinterpret_cast<char*>(&byte), 1);
        int unUsedBite = (byte >> 3)*std::pow(2, 3);
        if (byte >> 7) {
            tmpLocalColorTableSize =  3*std::pow(2, (byte - unUsedBite )+ 1);
            colorTable_pair_pos_size[pos_key++] = {file.tellg(), tmpLocalColorTableSize};
        }
    }

    file.seekg(tmpLocalColorTableSize + 1, std::ios::cur);

    uint8_t subBlockSize;
    do {
        file.read(reinterpret_cast<char*>(&subBlockSize), 1);
        if (fileSize <= ((int)file.tellg() + subBlockSize)) {
            return ;
        }
        file.seekg(subBlockSize, std::ios::cur);
    } while (subBlockSize != 0 && !file.eof());
}

     file.close();
}

Gif::Gif(std::string path) : path(path) {
    if (Gif::isGif(path))
     setInfo();
}


auto Gif::isGif(std::string path) -> bool {
    auto file = std::fstream(path, std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
      return false;
    }

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

Gif::~Gif() = default;

  auto Gif::isIntroduser(std::fstream& file) -> bool {
      uint8_t byte;
      file.read(reinterpret_cast<char*>(&byte), 1);
      return (int)byte == 0x21;
  }

auto Gif::isReadable(std::fstream &file) -> bool {

      auto possition = file.tellg();

      int size = m_message.signature.size();
      auto str = std::string();
      auto character = uint8_t();
      auto bit = uint8_t();
      auto counter = 0;;

      auto byter = int();
        bool end = false;

      for (const auto& [key, value] : colorTable_pair_pos_size) {
          file.seekg(value.first, std::ios::beg);
          if (end)
              break;
          for (int i = 0; i < value.second; i++) {
              if (counter == 8) {
                  byter++;
                  counter = 0;
                  str += (char)(character);
                  if (str.size() == size) {
                      end = true;
                      break;
                  }
                  character = 0;
              }
              file.read(reinterpret_cast<char*>(&bit), 1);
              character += std::pow(2, counter++)*(bit & 1);
          }
      }

      file.seekg(possition);
      if (str == m_message.signature) {
          return true;
      }
      file.close();
      return false;
}

auto  Gif::extentionSize(int ext) -> int {
    switch (ext) {
        case gf::EXTENSIONS::ApplicationExtensionLabel: return 11;
        case gf::EXTENSIONS::CommentLabel: return 0;
        case gf::EXTENSIONS::GraphicsControlExtension: return 4;
        default: return 404;
    }
}

auto Gif::isLastbitSame(uint8_t const & a, uint8_t const &b) -> bool {
      return a%2 == b%2;
}


auto Gif::setMessage(Message message) -> void {
      auto file = std::fstream(path, std::ios::binary | std::ios::in | std::ios::out);
      if (!file.is_open()) {
          std::cerr << "Could not open file: " << path << std::endl;
          return;
      }

      if (!Gif::isGif(path)) {
          file.close();
          return;
      }

      if (!isValid( message)) {
          std::cerr << "Can't set a message, too many characters:  " << path << std::endl;
      }

      auto bit = uint8_t();
      uint8_t* secretMas = message.getMessageBinary();

      int counter = 0;
      int byter = 0;

      bool end = false;

      for (const auto& [key, value] : colorTable_pair_pos_size) {
        file.seekg(value.first, std::ios::beg);
          for (int i = 0; i < value.second; i++) {
              if (counter == 8) {
                  if (byter++ > message.encryptSize()){
                      end = true;
                      break;
                  }
                  counter = 0;
              }
              file.read(reinterpret_cast<char*>(&bit), 1);
                if (!isLastbitSame(secretMas[byter] >> counter, bit)) {
                    file.seekg(-1, std::ios::cur);
                    if (bit % 2)
                        bit &= ~1;
                    else bit |= 1;
                    file.write(reinterpret_cast<char*>(&bit), 1);
                }

              counter++;
          }
          if (end)
              break;
      }
      delete[] secretMas;
      file.close();
  }
auto Gif::getMessage() -> Message {
      auto file = std::fstream(path,std::ios::binary | std::ios::in | std::ios::out);
      if (!file.is_open()) {
          std::cerr << "Could not open file: " << path << std::endl;
          return Message();
      }


        if (!Gif::isGif(path)) {
            file.close();
            return Message();
        }

      if (!isReadable(file)) {
          std::cerr << "File does not contain a message: " << path << std::endl;
          return Message();
      }
      int counter = 0;
      int byter = 0;
      std::string str = "";
      uint8_t character = 0;
      uint8_t bit;

      bool end = false;

      for (const auto& [key, value] : colorTable_pair_pos_size) {
          file.seekg(value.first, std::ios::beg);
          if (end)
              break;
          for (int i = 0; i < value.second; i++) {
              if (counter == 8) {
                  byter++;
                  counter = 0;
                  str += (char)(character);
                  if (character == 0) {
                      end = true;
                      break;
                  }
                  character = 0;
              }
                file.read(reinterpret_cast<char*>(&bit), 1);
                character += std::pow(2, counter++)*(bit & 1);
              }
          }
      file.close();
      auto endMes = Message(str);
      Message::removeSignature(endMes);
      return endMes;

}

auto Gif::skipExtension(int ext, std::fstream& file) -> void {
      switch (ext) {
          uint8_t byte;
          case 11: {
                file.seekg(ext + 1, std::ios::cur);
                file.read(reinterpret_cast<char*>(&byte), 1);
              file.seekg((int)byte + terminator_size, std::ios::cur);
              return;
          }
          case 0: {
                file.read(reinterpret_cast<char*>(&byte), 1);
                file.seekg((int)byte + terminator_size, std::ios::cur);
              return;
          }
          case 4: {
              file.read(reinterpret_cast<char*>(&byte), 1);
              file.seekg((int)byte +  terminator_size, std::ios::cur);
              return;
          }
          case 1 : {
              file.read(reinterpret_cast<char*>(&byte), 1);
              file.seekg((int)byte + terminator_size, std::ios::cur);
          }
         case 404: {
              do {
                  file.read(reinterpret_cast<char*>(&byte), 1);
                  if (byte == 0x21 || byte == 0x2C) {
                        file.seekg(-1, std::ios::cur);
                      return;
                  }
              }while (!file.eof());
          }
      }
  }

  auto Gif::resolution(std::string path) -> std::string {
      uint8_t byte;
      int width = 0, height = 0;
      auto file = std::fstream(path,std::ios::binary | std::ios::in);
      file.seekg(headerSize, std::ios::beg);
      file.read(reinterpret_cast<char*>(&width), 2);
      file.read(reinterpret_cast<char*>(&height),2);
        return std::to_string(width)+"x"+std::to_string(height);
  }
