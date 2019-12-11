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
#include "task.h"

namespace iiran {
    std::string format_task_result(int32_t task_id, const std::string &task_res, const std::string &file_path);

    class Scan {
    public:
        explicit Scan() = default;

        explicit Scan(std::vector<Task *> tasks);

        void set_path(std::string path);

        void init();

        void run();

        virtual ~Scan();

    protected:
        std::string m_file_path;
    private:
        static const std::string::size_type FILE_MAX_SIZE = 100'000'000;

        std::string m_text;
        std::vector<Task *> m_tasks;
        std::vector<std::string> m_task_res;
    };


    enum class FileType : int {
        JavaScript,
        TypeScript,
        Cpp,
        Python,
        Golang,
        Rust,
        Swift,
        Unknown,
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

    Scan *create_scan(std::string filePath);

    FileType get_filetype(const std::string &filePath);

}


#endif //CUDDLY_POTATO_SCAN_H
