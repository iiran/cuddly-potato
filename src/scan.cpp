
#include "scan.h"
#include "file_type.h"
#include <map>
#include <condition_variable>

//
// Created by yiran feng on 2019/12/9.
//

namespace iiran {


    void Scan::init(std::string content) {
        m_text = std::move(content);
    }


    std::vector<std::string> Scan::run() {
        unsigned max_worker_num = std::thread::hardware_concurrency();

        std::vector<std::thread> workers(max_worker_num);

        for (const auto &t : m_tasks) {
            workers.emplace_back([&]() {
                std::unique_lock worker_lock{m_worker_run_mtx};
                m_worker_run_cv.wait(worker_lock, [&] { return m_worker_num < max_worker_num; });
                worker_lock.unlock();
                ++m_worker_num;
                std::string task_res = t->operator()(m_text);
                std::string format_res = format_task_result(t->get_id(), task_res, m_file_path);
                {
                    std::lock_guard lock(m_task_res_mtx);
                    m_task_result.push_back(std::move(format_res));
                }
                --m_worker_num;
                m_worker_run_cv.notify_one();
            });
        }

        for (auto &w : workers) {
            if (w.joinable()) {
                w.join();
            }
        }
        return m_task_result;
    }

    Scan::Scan(std::vector<Task *> tasks) : m_worker_num{0} {
        m_tasks = std::move(tasks);
    }

    Scan::~Scan() {
        for (auto &t :m_tasks) {
            delete t;
        }
    }


    CppScan::CppScan() : Scan({new CountLine(), new CountSemicolon()}) {}

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
        char buf[1024]{0};
        std::snprintf(buf, sizeof buf, R"({"id":%u,"res":"%s","file":"%s"})", task_id, task_res.c_str(),
                      file_path.c_str());
        return std::string{buf};
    }

}
