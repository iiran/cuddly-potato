
#include "scan.h"
#include <map>

//
// Created by yiran feng on 2019/12/9.
//

namespace iiran {

    static std::map<std::string, FileType> filetype_map = {
            {"cpp", FileType::Cpp},
            {"py",  FileType::Python},
    };

    void Scan::set_path(std::string path) {
        m_file_path = std::move(path);
    }

    void Scan::init() {
        if (m_file_path.length() == 0)
            throw std::logic_error("file path not set");

        std::ifstream m_fs{};
        m_fs.open(m_file_path, std::fstream::in | std::fstream::ate);
        auto f_size = m_fs.tellg();
        if (f_size > Scan::FILE_MAX_SIZE) {
            if (m_fs.is_open()) m_fs.close();
            throw std::out_of_range("file is too large");
        }

        m_fs.seekg(0);
        m_text = {std::istreambuf_iterator<char>{m_fs}, std::istreambuf_iterator<char>{}};

        if (m_fs.is_open()) m_fs.close();
    }


    void Scan::run() {
        unsigned cpu_num = std::thread::hardware_concurrency();

        for (const auto &t : m_tasks) {
            std::string task_res = std::move(t->operator()(m_text));
            m_task_res.push_back(format_task_result(t->get_id(), task_res, m_file_path));
        }

        for (const auto &r : m_task_res) {
            std::cout << r << std::endl;
        }
    }

    Scan::Scan(std::vector<Task *> tasks) {
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
                throw std::invalid_argument("file not exist");
        }
    }

    FileType get_filetype(const std::string &file_path) {
        const size_t idx = file_path.find_last_of('.');
        if (idx == std::string::npos) return FileType::Unknown;

        std::string suffix = file_path.substr(idx + 1);
        if (auto it{filetype_map.find(suffix)}; it != std::end(filetype_map)) {
            return it->second;
        } else {
            return FileType::Unknown;
        }
    }

    std::string format_task_result(int32_t task_id, const std::string &task_res, const std::string &file_path) {
        char buf[1024]{0};
        std::snprintf(buf, sizeof buf, R"({id:%u,res:"%s",file:"%s")", task_id, task_res.c_str(), file_path.c_str());
        return std::string{buf};
    }

}
