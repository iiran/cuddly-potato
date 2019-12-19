//
// Created by yiran feng on 2019/12/9.
//

#ifndef CUDDLY_POTATO_SCAN_H
#define CUDDLY_POTATO_SCAN_H

#include <string>
#include <fstream>
#include <exception>
#include <functional>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>

class Task;

namespace iiran {
    std::string format_task_result(int32_t task_id, const std::string &task_res, const std::string &file_path);

    class Scan {
    public:
        explicit Scan() = default;

        explicit Scan(std::vector<Task *> tasks);

        void init(std::string content) { m_text = std::move(content); };

        std::vector<std::string> run();

        virtual ~Scan() = default;

    protected:
        std::string m_file_path;
    private:
        std::string m_text;
        std::vector<std::unique_ptr<Task>> m_tasks;
        std::vector<std::string> m_task_result;
        std::mutex m_task_res_mtx;
        std::atomic_uint8_t m_worker_num{0};
        std::mutex m_worker_run_mtx;
        std::condition_variable m_worker_run_cv;
    };

    class CppScan : public Scan {
    public:
        explicit CppScan();

        explicit CppScan(std::string filepath);
    };

    class PythonScan : public Scan {
    public:
        explicit PythonScan();

        explicit PythonScan(std::string filepath);
    };

    class TextScan : public Scan {
    public:
        explicit TextScan() ;

        explicit TextScan(std::string filepath);
    };

    Scan *create_scan(std::string filePath);

}


#endif //CUDDLY_POTATO_SCAN_H
