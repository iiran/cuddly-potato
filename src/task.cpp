//
// Created by yiran feng on 2019/12/10.
//

#include <sstream>
#include "task.h"

namespace iiran {
    int64_t count_char(const std::string &s, const char &c) {
        std::istringstream reader{s};
        int64_t n{0};
        for (auto &t : s) {
            if (t == c) ++n;
        }
        return n;
    }


    int CountLine::get_id() const {
        return CountLine::ID;
    }


    std::string CountLine::operator()(const std::string &text) {
        return std::to_string(count_char(text, '\n'));
    }


    int CountSemicolon::get_id() const {
        return CountSemicolon::ID;
    }

    std::string CountSemicolon::operator()(const std::string &text) {
        return std::to_string(count_char(text, ';'));
    }

}

