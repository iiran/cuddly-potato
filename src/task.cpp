//
// Created by yiran feng on 2019/12/10.
//

#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <iostream>
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

    int64_t count_line(const std::string &s) {
        return count_char(s, '\n') + 1;
    }


    int CountLine::get_id() const noexcept {
        return CountLine::ID;
    }


    std::string CountLine::operator()(const std::string &text) {
        return std::to_string(count_line(text));
    }


    int CountSemicolon::get_id() const noexcept {
        return CountSemicolon::ID;
    }

    std::string CountSemicolon::operator()(const std::string &text) {
        return std::to_string(count_char(text, ';'));
    }


    std::string Task::merge_result(std::vector<std::string> results) {
        return std::string();
    }


    int CountCCommentLine::get_id() const noexcept {
        return CountCCommentLine::ID;
    }

    bool is_in_comment(SlashStat state) {
        return state == SlashStat::Double || state == SlashStat::Block || state == SlashStat::BlockWait;
    }

    SlashStat next_slash_stat(SlashStat state, char c) {
        if (state == SlashStat::Zero) {
            if (c == '/') {
                state = SlashStat::Single;
            } else {}
        } else if (state == SlashStat::Single) {
            if (c == '/') {
                state = SlashStat::Double;
            } else if (c == '*') {
                state = SlashStat::Block;
            } else if (c == '\n') {
                state = SlashStat::Zero;
            } else {}
        } else if (state == SlashStat::Double) {
            if (c == '\n') {
                state = SlashStat::Zero;
            } else {}
        } else if (state == SlashStat::Block) {
            if (c == '*') {
                state = SlashStat::BlockWait;
            } else {}
        } else if (state == SlashStat::BlockWait) {
            if (c == '/') {
                state = SlashStat::Zero;
            } else {
                state = SlashStat::Block;
            }
        } else {
            throw std::logic_error("unreachable slash status");
        }
        return state;
    }

    std::string get_compensate_by_state_change(SlashStat last, SlashStat now) {
        if (last == SlashStat::Single) {
            if (now == SlashStat::Double) {
                return "//";
            } else if (now == SlashStat::Block) {
                return "/*";
            }
        }
        return "";
    }

    std::string CountCCommentLine::operator()(const std::string &text) {
        SlashStat lc_stat = SlashStat::Zero;

        uint64_t line_cmt_n{0};
        std::string cmt_in_line{};
        cmt_in_line.reserve(Task::TASK_INIT_LINE_LEN);

        for (const auto &c : text) {
            if (c == '\n' || c == '\r') {
                cmt_in_line.erase(cmt_in_line.find_last_not_of(" \n\r\t") + 1);

                if (!cmt_in_line.empty()) {
                    ++line_cmt_n;
                }
                cmt_in_line.clear();
            } else if (is_in_comment(lc_stat)) {
                cmt_in_line += c;
            }

            // last step: change stat
            SlashStat old_stat = lc_stat;
            lc_stat = next_slash_stat(lc_stat, c);
            cmt_in_line += get_compensate_by_state_change(old_stat, lc_stat);
        }
        if (!cmt_in_line.empty()) {
            ++line_cmt_n;
        }

        return std::to_string(line_cmt_n);
    }

    int CountBlankLine::get_id() const noexcept {
        return ID;
    }

    std::string CountBlankLine::operator()(const std::string &text) {

        uint64_t line_blank_n{0};
        uint64_t not_blank_ch_n{0};
        for (const auto &c : text) {
            if (c == '\n' || c == '\r') {
                if (not_blank_ch_n == 0) {
                    ++line_blank_n;
                }
                not_blank_ch_n = 0;
            } else if (c != '\t' && c != ' ') {
                ++not_blank_ch_n;
            }
        }
        if (not_blank_ch_n > 0) {
            ++line_blank_n;
        }

        return std::to_string(line_blank_n);
    }

    std::string VariableNameStatistic::operator()(const std::string &text) {

        std::map<std::string, int64_t> word_map{};
        std::string current_word{};

        auto update_word = [&]() {
            if (!current_word.empty()) {
                ++word_map[current_word];
                current_word.clear();
            }
        };

        char last_c = '\0';
        for (const auto &c : text) {
            if ((last_c == ' ' || last_c == '\t' || last_c == '\n') && (std::isalpha(c) || c == '_')) {
                update_word();
            }
            if (std::isalpha(c) || c == '_' || (std::isalnum(c) && !current_word.empty())) {
                current_word += c;
            }
            last_c = c;
        }
        update_word();

        std::string res{'['};
        for (const auto &word : word_map) {
            if (word.second > 3) {
                res += '"' + word.first + R"(",")" + std::to_string(word.second) + R"(",)";
            }
        }
        res.back() = ']';
        return res;
    }


}

