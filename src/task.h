//
// Created by yiran feng on 2019/12/10.
//

#ifndef JEAN_TASK_H
#define JEAN_TASK_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <map>
#include <iostream>

namespace iiran {

enum class TaskType : int32_t {
  CountLine,
  CountSemicolon,
  CountCCommentLine,
  CountBlankLine,
  CountCodeLine,
  VariableNameStatistic,
  GetIndentStyle,
};

enum class TaskResultOrigin : int32_t {
  Scan,
  Merge,
};

struct TaskResult {
  TaskResult() = default;
  explicit TaskResult(std::string result, int32_t id) : result{std::move(result)}, task_id{id} {}
  TaskResult &operator+=(const TaskResult &other);
  int32_t task_id{-1};
  int32_t file_type{-1};
  int32_t origin{static_cast<int32_t>(TaskResultOrigin::Scan)};
  std::string result{};
};

void merge_num_result(TaskResult &total_result, const TaskResult &single_result);

template<typename T, typename N>
static std::string format_map_num_str(const std::map<T, N> &m, int64_t min) {
  std::string res{};

  for (const auto &e : m) {
    if (e.second >= min) {
      res += std::to_string(e.first) + ' ' + std::to_string(e.second) + ' ';
    }
  }
  return res;
}

template<typename N>
std::string format_map_num_str(const std::map<std::string, N> &m, int64_t min) {
  std::string res{};
  for (const auto &e : m) {
    if (e.second >= min) {
      res += e.first + ' ' + std::to_string(e.second) + ' ';
    }
  }
  return res;
}

void extract_map_num_result(std::map<std::string, int64_t> &m, const TaskResult &single_result);

void merge_map_num_result(TaskResult
                          &total_result,
                          const TaskResult &single_result
);

class Task {
 public:
  [[nodiscard]] virtual int get_id() const noexcept = 0;

  virtual TaskResult operator()(const std::string &text) = 0;

  virtual ~Task() = default;

  static const int32_t TASK_LINE_MAX{10'000};
  static const size_t TASK_INIT_LINE_LEN{1'000};
};
//////
class CountLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return CountLine::ID; }

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_num_result(total_result, single_result);
  }

  TaskResult operator()(const std::string &text) override;

 private:
  static const int ID = static_cast<int>(TaskType::CountLine);
};
///////
class CountSemicolon : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return CountSemicolon::ID; }

  TaskResult operator()(const std::string &text) override;

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_num_result(total_result, single_result);
  }

 private:
  static const int ID = static_cast<int>(TaskType::CountSemicolon);
};
//////
class CountCCommentLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return CountCCommentLine::ID; }

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_num_result(total_result, single_result);
  }

  TaskResult operator()(const std::string &text) override;

 private:
  static const int ID = static_cast<int>(TaskType::CountCCommentLine);
};

int64_t count_char(const std::string &s, const char &c);

int64_t count_line(const std::string &s);

enum class SlashStat : uint8_t {
  Zero,
  Single,    // -> /
  Double,    // -> // ...
  Block,     // -> /* ...
  BlockWait  // -> /* ... *
};

SlashStat next_slash_stat(SlashStat state, char c);

std::string get_compensate_by_state_change(SlashStat last, SlashStat now);

bool is_in_comment(SlashStat state);
/////
class CountBlankLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_num_result(total_result, single_result);
  }

  TaskResult operator()(const std::string &text) override;

 private:
  static const int ID = static_cast<int>(TaskType::CountBlankLine);
};
/////
class VariableNameStatistic : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  TaskResult operator()(const std::string &text) override;

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_map_num_result(total_result, single_result);
  }

 private:
  static const int ID{static_cast<int>(TaskType::VariableNameStatistic)};
};
/////
template<class CommentStyle>
class CountCodeLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_num_result(total_result, single_result);
  }

  TaskResult operator()(const std::string &text) override {
    std::unique_ptr<Task> cmt_task = std::make_unique<CountCCommentLine>();
    TaskResult cmt_line = std::move(cmt_task->operator()(text));

    std::unique_ptr<Task> blank_task = std::make_unique<CountBlankLine>();
    TaskResult blank_line = std::move(blank_task->operator()(text));

    int64_t total = count_line(text);
    int64_t base = 64;
    int64_t cmt = std::strtol(cmt_line.result.c_str(), nullptr, base);
    int64_t blank = std::strtol(blank_line.result.c_str(), nullptr, base);

    assert(total - cmt - blank >= 0);

    TaskResult r{
        std::to_string(total - cmt - blank),
        get_id()
    };
    return r;
  };

 private:
  static const int ID{static_cast<int>(TaskType::CountCodeLine)};
};
/////
class GetIndentStyle : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  static void merge_result(TaskResult &total_result, const TaskResult &single_result) {
    merge_map_num_result(total_result, single_result);
  }

  TaskResult operator()(const std::string &text) override;
 private:
  static const int ID{static_cast<int>(TaskType::GetIndentStyle)};
};

static void merge_task_result(TaskType typ, TaskResult &left, const TaskResult &right) {
  if (TaskType::CountLine == typ) {
    CountLine::merge_result(left, right);
  } else if (TaskType::CountSemicolon == typ) {
    CountSemicolon::merge_result(left, right);
  } else if (TaskType::CountCCommentLine == typ) {
    CountCCommentLine::merge_result(left, right);
  } else if (TaskType::CountBlankLine == typ) {
    CountBlankLine::merge_result(left, right);
  } else if (TaskType::CountCodeLine == typ) {
    CountCodeLine<CountCCommentLine>::merge_result(left, right);
  } else if (TaskType::VariableNameStatistic == typ) {
    VariableNameStatistic::merge_result(left, right);
  } else if (TaskType::GetIndentStyle == typ) {
    GetIndentStyle::merge_result(left, right);
  } else {
    std::cout << "unknown task_id" << std::endl;
    return;
  }
}

static void merge_task_result_by_id(int32_t
                                    task_id, TaskResult &left, const TaskResult &right) {
  auto typ = static_cast<TaskType>(task_id);
  return merge_task_result(typ, left, right);
}

static std::string get_task_desc(TaskType tt) {
  static const std::map<TaskType, std::string> task_meta_map{
      {TaskType::CountLine, "count line"},
      {TaskType::CountSemicolon, "count semicolon"},
      {TaskType::CountCCommentLine, "count comment line"},
      {TaskType::CountBlankLine, "count blank line"},
      {TaskType::CountCodeLine, "count code line"},
      {TaskType::VariableNameStatistic, "variable name analysis"},
      {TaskType::GetIndentStyle, "indent analysis"},
  };
  return task_meta_map.at(tt);
}

static std::string get_task_desc(int32_t task_id) {
  return get_task_desc(static_cast<TaskType>(task_id));
}

}  // namespace iiran

#endif  // JEAN_TASK_H
