//
// Created by yiran feng on 2019/12/12.
//

#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include "research.h"
#include "scan.h"
#include "file_type.h"

namespace iiran {

    // default max concurrency == logic cpu core
    Research::Research(std::string out_path) : m_out_path{std::move(out_path)},
                                               m_max_concurrency{std::thread::hardware_concurrency()} {}

    void Research::export_result() {
        std::ofstream fs{};
        fs.open(m_out_path, std::ios_base::out);
        std::lock_guard lock(m_result_mtx);
        size_t result_n = m_results.size();
        try {
            fs << '[';
            for (int i = 0; i < result_n; i++) {
                fs << m_results.at(i);
                if (i != result_n - 1) {
                    fs << ',';
                }
            }
            fs << ']';
        } catch (std::exception &e) {
            goto release_file;
        }

        // final
        release_file:
        if (fs.is_open()) {
            fs.close();
        }
    }

    Research &Research::run() {
        std::vector<std::thread> workers;

        for (const auto &path : m_support_file_paths) {
            workers.emplace_back([&]() {
                std::unique_lock worker_lock{m_worker_run_mtx};
                m_worker_run_cv.wait(worker_lock, [&] { return m_worker_num < m_max_concurrency; });
                ++m_worker_num;
                worker_lock.unlock();

                Scan *s = create_scan(path);
                s->init(get_file_content(path));
                std::vector<std::string> res = std::move(s->run());
                {
                    std::lock_guard lock{m_result_mtx};
                    m_results.insert(m_results.end(), std::make_move_iterator(res.begin()),
                                     std::make_move_iterator(res.end()));
                }
                --m_worker_num;
                m_worker_run_cv.notify_one();
            });
        }

        auto support_file_path_size = m_support_file_paths.size();
        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (workers.size() == support_file_path_size) {
                for (auto &w : workers) {
                    assert(w.joinable());
                    w.join();
                }
                break;
            }
        }
        return *this;
    }

    void get_all_supported_local_recursive(const std::string &path, std::vector<std::string> &v_path) {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            std::string entry_path = entry.path();
            if (entry.is_directory()) {
                get_all_supported_local_recursive(entry_path, v_path);
            } else {
                if (get_filetype(entry_path) != FileType::Unknown) {
                    v_path.push_back(entry_path);
                }
            }
        }
    }


    LocalResearch::LocalResearch(std::string out_path) : Research(std::move(out_path)) {}

    LocalResearch &LocalResearch::add_target(std::string target) {
        m_root_dir = std::move(target);
        return *this;
    }

    LocalResearch &LocalResearch::init() {
        if (m_root_dir.length() == 0)
            throw std::logic_error("root path not set");

        std::vector<std::string> supported_file_paths{};
        get_all_supported_local_recursive(m_root_dir, supported_file_paths);
        m_support_file_paths = std::move(supported_file_paths);
        return *this;
    }

    std::string LocalResearch::get_file_content(const std::string &path) {
        if (path.length() == 0)
            throw std::logic_error("file path not set");

        std::ifstream m_fs{};
        m_fs.open(path, std::fstream::in | std::fstream::ate);
        auto f_size = m_fs.tellg();
        if (f_size > Research::FILE_MAX_SIZE) {
            if (m_fs.is_open()) m_fs.close();
            throw std::out_of_range("failed to read file"); // too large or not exist
        }

        m_fs.seekg(0);
        std::string text = {std::istreambuf_iterator<char>{m_fs}, std::istreambuf_iterator<char>{}};
        if (m_fs.is_open()) m_fs.close();
        return text;
    }

    RemoteResearch::RemoteResearch(std::string out_path) : Research(std::move(out_path)) {}

    RemoteResearch &RemoteResearch::add_target(std::string target) {
        m_root_url = std::move(target);
        return *this;
    }

    RemoteResearch &RemoteResearch::init() {
        const uint8_t network_speed_ratio = 2;
        m_max_concurrency = network_speed_ratio * std::thread::hardware_concurrency();
        //todo add files
        return *this;
    }

}