//
// Created by yiran feng on 2019/12/10.
//

#include "task.h"
#include <array>
#include <map>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace iiran {
int64_t count_char(const std::string &s, const char &c) {
  std::istringstream reader{s};
  int64_t n{0};
  for (auto &t : s) {
    if (t == c) ++n;
  }
  return n;
}

int64_t count_line(const std::string &s) { return count_char(s, '\n') + 1; }

TaskResult CountLine::operator()(const std::string &text) {
  TaskResult r{std::to_string(count_line(text)), get_id()};
  return r;
}

TaskResult CountSemicolon::operator()(const std::string &text) {
  TaskResult r{std::to_string(count_char(text, ';')), get_id()};
  return r;
}

bool is_in_comment(SlashStat state) {
  return state == SlashStat::Double || state == SlashStat::Block ||
      state == SlashStat::BlockWait;
}

SlashStat next_slash_stat(SlashStat state, char c) {
  if (state == SlashStat::Zero) {
    if (c == '/') {
      state = SlashStat::Single;
    } else {
    }
  } else if (state == SlashStat::Single) {
    if (c == '/') {
      state = SlashStat::Double;
    } else if (c == '*') {
      state = SlashStat::Block;
    } else if (c == '\n') {
      state = SlashStat::Zero;
    } else {
    }
  } else if (state == SlashStat::Double) {
    if (c == '\n') {
      state = SlashStat::Zero;
    } else {
    }
  } else if (state == SlashStat::Block) {
    if (c == '*') {
      state = SlashStat::BlockWait;
    } else {
    }
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
void extract_map_num_result(std::map<std::string, int64_t> &m, const TaskResult &single_result) {
  std::vector<std::string> sv{};
  boost::split(sv, single_result.result, boost::is_any_of(" "));
  std::string name{};
  int64_t times{};
  for (int i = 0; i < sv.size() - 1; i += 2) {
    m[sv[i]] = boost::lexical_cast<int64_t>(sv[i + 1]);
  }
}

void merge_num_result(TaskResult &total_result, const TaskResult &single_result) {
  total_result.result = std::to_string(atoi(total_result.result.c_str()) + atoi(single_result.result.c_str()));
}

void merge_map_num_result(TaskResult &total_result, const TaskResult &single_result) {
  std::map<std::string, std::int64_t> m{};
  extract_map_num_result(m, total_result);
  extract_map_num_result(m, single_result);
  total_result.result = format_map_num_str(m, 0);
}

TaskResult CountCCommentLine::operator()(const std::string &text) {
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

  TaskResult r{std::to_string(line_cmt_n), get_id()};
  return r;
}

TaskResult CountBlankLine::operator()(const std::string &text) {
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

  TaskResult r{std::to_string(line_blank_n), get_id()};
  return r;
}

TaskResult VariableNameStatistic::operator()(const std::string &text) {
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
    if ((last_c == ' ' || last_c == '\t' || last_c == '\n') &&
        (std::isalpha(c) || c == '_')) {
      update_word();
    }
    if (std::isalpha(c) || c == '_' ||
        (std::isalnum(c) && !current_word.empty())) {
      current_word += c;
    }
    last_c = c;
  }
  update_word();

  std::string res{format_map_num_str(word_map, 3)};

  TaskResult r{res, get_id()};
  return r;
}

TaskResult GetIndentStyle::operator()(const std::string &text) {
  char last_c = '\0';
  bool on_indent = false;
  int16_t space_len = 0;
  std::map<int16_t, int32_t> line_spaces = {};
  int16_t last_indent = 0;
  int16_t tab_num = 0;
  for (const auto &c : text) {
    if (on_indent) {
      if (c == ' ') {
        ++space_len;
      } else if (c == '\t') {
        ++tab_num; // 1tab == indent
        space_len += last_indent;
      } else {
        line_spaces[space_len]++;
        last_indent = space_len;
        space_len = 0;
        if (c != '\n') {
          on_indent = false;
        }
      }
    } else {
      if (c == '\n') {
        on_indent = true;
      }
    }
  }
  line_spaces[-1] = tab_num;
  std::string res{format_map_num_str(line_spaces, 1)};

  TaskResult r{res, get_id()};
  return r;
}
TaskResult &TaskResult::operator+=(const TaskResult &other) {
  if (this->task_id == other.task_id) {
    merge_task_result_by_id(this->task_id, *this, other);
  }
  return *this;
}
}  // namespace iiran
