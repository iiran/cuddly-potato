//
// Created by yiran feng on 2019/12/10.
//

#ifndef JEAN_TASK_H
#define JEAN_TASK_H

#include <string>
#include <vector>

namespace iiran {

enum class TaskType : int {
  CountLine,
  CountSemicolon,
  CountCCommentLine,
  CountBlankLine,
  CountCodeLine,
  VariableNameStatistic,
  GetIndentStyle,
};

class Task {
 public:
  [[nodiscard]] virtual int get_id() const noexcept = 0;

  virtual std::string operator()(const std::string &text) = 0;

  virtual ~Task() = default;

  // merge result generated
  virtual std::string merge_result(std::vector<std::string> results) {
    return std::string();
  }

  static const int32_t TASK_LINE_MAX{10'000};
  static const size_t TASK_INIT_LINE_LEN{1'000};
};

class CountLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return CountLine::ID; }

  std::string operator()(const std::string &text) override;

 private:
  static const int ID = static_cast<int>(TaskType::CountLine);
};

class CountSemicolon : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return CountSemicolon::ID; }

  std::string operator()(const std::string &text) override;

 private:
  static const int ID = static_cast<int>(TaskType::CountSemicolon);
};

class CountCCommentLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return CountCCommentLine::ID; }

  std::string operator()(const std::string &text) override;

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

class CountBlankLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  std::string operator()(const std::string &text) override;

 private:
  static const int ID = static_cast<int>(TaskType::CountBlankLine);
};

class VariableNameStatistic : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  std::string operator()(const std::string &text) override;

 private:
  static const int ID{static_cast<int>(TaskType::VariableNameStatistic)};
};

template<class CommentStyle>
class CountCodeLine : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  std::string operator()(const std::string &text) override {
    std::unique_ptr<Task> cmt_task = std::make_unique<CountCCommentLine>();
    std::string cmt_line = std::move(cmt_task->operator()(text));

    std::unique_ptr<Task> blank_task = std::make_unique<CountBlankLine>();
    std::string blank_line = std::move(blank_task->operator()(text));

    int64_t total = count_line(text);
    int64_t base = 64;
    int64_t cmt = std::strtol(cmt_line.c_str(), nullptr, base);
    int64_t blank = std::strtol(blank_line.c_str(), nullptr, base);

    assert(total - cmt - blank >= 0);

    return std::to_string(total - cmt - blank);
  };

 private:
  static const int ID{static_cast<int>(TaskType::CountCodeLine)};
};

class GetIndentStyle : public Task {
 public:
  [[nodiscard]] int get_id() const noexcept override { return ID; }

  std::string operator()(const std::string &text) override;
 private:
  static const int ID{static_cast<int>(TaskType::GetIndentStyle)};
};

}  // namespace iiran

#endif  // JEAN_TASK_H
