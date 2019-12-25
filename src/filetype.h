//
// Created by yiran feng on 2019/12/13.
//

#ifndef JEAN_FILETYPE_H
#define JEAN_FILETYPE_H

#include <string>
#include <map>

namespace iiran {
enum class FileType : int32_t {
  JavaScript,
  TypeScript,
  Cpp,
  Python,
  Golang,
  Rust,
  Swift,
  GitKeep,
  GitIgnore,
  Unknown,
};

enum class MetaType : int32_t {
  Describe
};

class File {
 public:
  static FileType get_filetype(const std::string &file_path) {
    static const std::map<std::string, FileType> filetype_map = {
        {"cpp", FileType::Cpp},
        {"cc", FileType::Cpp},
        {"cxx", FileType::Cpp},
        {"hpp", FileType::Cpp},
        {"h", FileType::Cpp},
        {"py", FileType::Python},
        {"gitkeep", FileType::GitKeep},
        {"gitignore", FileType::GitIgnore},
        {"swift", FileType::Swift},
        {"go", FileType::Golang},
        {"js", FileType::JavaScript},
        {"ts", FileType::TypeScript},
        {"rs", FileType::Rust},
    };
    const size_t idx = file_path.find_last_of('.');
    if (idx == std::string::npos) return FileType::Unknown;

    std::string suffix = file_path.substr(idx + 1);
    if (auto it{filetype_map.find(suffix)}; it != std::end(filetype_map)) {
      return it->second;
    } else {
      return FileType::Unknown;
    }
  }

  static std::string get_type_desc(const FileType ft) {
    return get_type_meta(ft, MetaType::Describe);
  }

  static std::string get_type_desc(int32_t ft) {
    return get_type_desc(static_cast<FileType>(ft));
  }

  static std::string get_type_meta(const FileType ft, const MetaType opt) {
    static const std::map<FileType, std::string> filetype_meta_map = {
        {FileType::Cpp, "C++"},
        {FileType::Python, "Python"},
        {FileType::GitKeep, "Git"},
        {FileType::GitIgnore, "Git"},
        {FileType::Swift, "Swift"},
        {FileType::Golang, "Go"},
        {FileType::JavaScript, "JavaScript"},
        {FileType::TypeScript, "TypeScript"},
        {FileType::Rust, "Rust"},
        {FileType::Unknown, "Unknown"}
    };
    if (opt == MetaType::Describe) {
      auto &desc = filetype_meta_map.at(ft);
      return desc;
    }
    return "";
  }
};

}  // namespace iiran

#endif  // JEAN_FILETYPE_H
