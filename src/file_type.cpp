//
// Created by yiran feng on 2019/12/13.
//

#include <map>
#include "file_type.h"

namespace iiran {

    static std::map<std::string, FileType> filetype_map = {
            {"cpp",       FileType::Cpp},
            {"cc",        FileType::Cpp},
            {"cxx",       FileType::Cpp},
            {"hpp",       FileType::Cpp},
            {"py",        FileType::Python},
            {"gitkeep",   FileType::GitKeep},
            {"gitignore", FileType::GitIgnore},
            {"swift",     FileType::Swift},
            {"go",        FileType::Golang},
            {"js",        FileType::JavaScript},
            {"ts",        FileType::TypeScript},
            {"rs",        FileType::Rust}
    };

    FileType get_filetype(const std::string &file_path) {
        const size_t idx = file_path.find_last_of('.');
        if (idx == std::string::npos) return FileType::Unknown;

        std::string suffix = file_path.substr(idx + 1);
        if (auto it{filetype_map.find(suffix)}; it != std::end(filetype_map)) {
            return it->second;
        } else {
            return FileType::Unknown;
        }
    }
}