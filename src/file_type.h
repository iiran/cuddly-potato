//
// Created by yiran feng on 2019/12/13.
//

#ifndef CUDDLY_POTATO_FILE_TYPE_H
#define CUDDLY_POTATO_FILE_TYPE_H

#include <string>

namespace iiran {
    enum class FileType : int {
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

    FileType get_filetype(const std::string &filePath);
}

#endif //CUDDLY_POTATO_FILE_TYPE_H
