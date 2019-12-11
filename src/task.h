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
    };

    class Task {
    public:
        virtual int get_id() const = 0;

        virtual std::string operator()(const std::string &text) = 0;

        virtual ~Task() = default;
    };

    class CountLine : public Task {
    public:
        int get_id() const override;

        std::string operator()(const std::string &text) override;

    private:
        static const int ID = static_cast<int> (TaskType::CountLine);
    };

    class CountSemicolon : public Task {
    public:
        int get_id() const override;

        std::string operator()(const std::string &text) override;

    private:
        static const int ID = static_cast<int> (TaskType::CountSemicolon);
    };

    int64_t count_char(const std::string &s, const char &c);

}

#endif //CUDDLY_POTATO_TASK_H
