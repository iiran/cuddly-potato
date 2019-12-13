
#include <map>
#include "file_type.h"
#include "task.h"
#include "scan.h"

//
// Created by yiran feng on 2019/12/9.
//

namespace iiran {


    std::vector<std::string> Scan::run() {
        unsigned max_worker_num = std::thread::hardware_concurrency();

        std::vector<std::thread> workers;

        for (const auto &t : m_tasks) {
            workers.emplace_back([&]() {
                std::unique_lock worker_lock{m_worker_run_mtx};
                m_worker_run_cv.wait(worker_lock, [&] { return m_worker_num < max_worker_num; });
                ++m_worker_num;
                worker_lock.unlock();
                std::string task_res = t->operator()(m_text);
                std::string format_res = format_task_result(t->get_id(), task_res, m_file_path);
                {
                    std::lock_guard lock(m_task_res_mtx);
                    m_task_result.emplace_back(std::move(format_res));
                }
                --m_worker_num;
                m_worker_run_cv.notify_one();
            });
        }

        auto task_size = m_tasks.size();
        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (workers.size() == task_size) {
                for (auto &w : workers) {
                    assert(w.joinable());
                    w.join();
                }
                break;
            }
        }
        return m_task_result;
    }

    Scan::Scan(std::vector<Task *> tasks) {
        for (const auto &pt: tasks) {
            m_tasks.emplace_back(std::unique_ptr<Task>(pt));
        }
    }


    CppScan::CppScan() : Scan({
                                      new CountLine(),
                                      new CountSemicolon(),
                                      new CountCCommentLine(),
                                      new CountBlankLine(),
                                      new CountCodeLine<CountCCommentLine>(),
                                      new VariableNameStatistic(),
                              }) {}

    CppScan::CppScan(std::string file_path) : CppScan() {
        m_file_path = std::move(file_path);
    }

    PythonScan::PythonScan() : Scan({new CountLine()}) {}

    PythonScan::PythonScan(std::string file_path) : PythonScan() {
        m_file_path = std::move(file_path);
    }

    Scan *create_scan(std::string file_path) {
        FileType ft = get_filetype(file_path);
        switch (ft) {
            case FileType::Cpp:
                return new CppScan(std::move(file_path));
            case FileType::Python:
                return new PythonScan(std::move(file_path));
            default:
                throw std::invalid_argument("unsupported type");
        }
    }


    std::string format_task_result(int32_t task_id, const std::string &task_res, const std::string &file_path) {
        char buf[Task::TASK_LINE_MAX]{0};
        assert(task_res.length() < Task::TASK_LINE_MAX);
        std::snprintf(buf, sizeof buf, R"({"id":%u,"res":%s,"file":"%s"})", task_id, task_res.c_str(),
                      file_path.c_str());
        return std::string{buf};
    }

}
