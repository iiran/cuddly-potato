//
// Created by yiran feng on 2019/12/10.
//

#ifndef CUDDLY_POTATO_TASK_H
#define CUDDLY_POTATO_TASK_H


#include <string>


namespace iiran {

    enum class TaskType : int {
        CountLine,
        CountSemicolon,
        CountCCommentLine,
        CountBlankLine,
        CountCodeLine,
        VariableNameStatistic,
//        GetIndentStyle,
//        IndentStatistic,
//        LineBreakStatistic,
//        CurlyBraceStatistic,
//        CodeRepeatStatistic,
    };

    class Task {
    public:

        [[nodiscard]] virtual int get_id() const noexcept = 0;

        virtual std::string operator()(const std::string &text) = 0;

        virtual ~Task() = default;

        virtual std::string merge_result(std::vector<std::string> results);

        static const int32_t TASK_LINE_MAX{1'0000};
        static const size_t TASK_INIT_LINE_LEN{1'000};
    };

    class CountLine : public Task {
    public:

        [[nodiscard]] int get_id() const noexcept override;


        std::string operator()(const std::string &text) override;

    private:
        static const int ID = static_cast<int> (TaskType::CountLine);
    };

    class CountSemicolon : public Task {
    public:

        [[nodiscard]] int get_id() const noexcept override;


        std::string operator()(const std::string &text) override;

    private:
        static const int ID = static_cast<int> (TaskType::CountSemicolon);
    };

    class CountCCommentLine : public Task {
    public:
        [[nodiscard]] int get_id() const noexcept override;

        std::string operator()(const std::string &text) override;

    private:
        static const int ID = static_cast<int>( TaskType::CountCCommentLine);
    };

    int64_t count_char(const std::string &s, const char &c);

    int64_t count_line(const std::string &s);

    enum class SlashStat : uint8_t {
        Zero,
        Single,       // -> /
        Double,       // -> // ...
        Block,        // -> /* ...
        BlockWait     // -> /* ... *
    };

    SlashStat next_slash_stat(SlashStat state, char c);

    std::string get_compensate_by_state_change(SlashStat last, SlashStat now);

    bool is_in_comment(SlashStat state);

    class CountBlankLine : public Task {
    public:
        [[nodiscard]] int get_id() const noexcept override;

        std::string operator()(const std::string &text) override;

    private:
        static const int ID = static_cast<int>( TaskType::CountBlankLine);
    };

    class VariableNameStatistic : public Task {
    public:
        [[nodiscard]] int get_id() const noexcept override { return ID; }


        std::string operator()(const std::string &text) override;

    private :
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
            int64_t cmt = std::strtol(cmt_line.c_str(), nullptr, 64);
            int64_t blank = std::strtol(blank_line.c_str(), nullptr, 64);

            assert(total - cmt - blank >= 0);

            return std::to_string(total - cmt - blank);
            return "";
        };

    private:
        static const int ID{static_cast<int>(TaskType::CountCodeLine)};
    };

}

#endif //CUDDLY_POTATO_TASK_H
