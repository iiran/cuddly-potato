//
// Created by yiran feng on 2019/12/12.
//

#ifndef JEAN_RESEARCH_H
#define JEAN_RESEARCH_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include "filetype.h"
#include "scan.h"

namespace iiran {
class Research {
 protected:
  std::vector<std::string> m_support_file_paths;
  std::string m_out_path;

 private:
  std::vector<TaskResult> m_results;
  std::mutex m_result_mtx;
  std::atomic_uint8_t m_worker_num{0};
  std::mutex m_worker_run_mtx;
  std::condition_variable m_worker_run_cv;

 public:
  static const std::string::size_type FILE_MAX_SIZE = 100'000'00;

  Research() = default;

  explicit Research(std::string out_path) : m_out_path{std::move(out_path)} {};

  virtual uint32_t get_max_concurrent() {
    return std::thread::hardware_concurrency();
  }

  void print_merged_result() {
    for (const auto &r : m_results) {
      if (static_cast<TaskResultOrigin>(r.origin) == TaskResultOrigin::Merge) {
        std::cout << "file-type: " << File::get_type_desc(r.file_type) << ", task: " << get_task_desc(r.task_id)
                  << ", result: "
                  << r.result << std::endl;
      }
    }
  }

  void merge_type_result() {
    std::map<int32_t, std::map<int32_t, TaskResult>> file_task_res{}; // file-type <-+> task-type <--> task-result
    for (const auto &t_result: m_results) {
      int32_t ft = t_result.file_type;
      int32_t tt = t_result.task_id;
      if (file_task_res.find(ft) == file_task_res.end()) {
        file_task_res[ft] = {};
      };
      auto &task_map = file_task_res[ft];
      if (task_map.find(tt) == task_map.end()) {
        task_map[tt] = t_result;
      } else {
        task_map[tt] += t_result;
      }
    }

    for (auto &task_m : file_task_res) {
      for (auto &task_res : task_m.second) {
        task_res.second.origin = static_cast<int32_t>(TaskResultOrigin::Merge);
        m_results.emplace_back(std::move(task_res.second));
      }
    }
  }

  virtual ~Research() = default;

/**
 * - add starting point (project root directory / github repo main page)
 * @param starting point (project root directory / github repo main page)
 * @return self
 */
  virtual Research &add_target(std::string target) = 0;

/**
 * - Determine the number of concurrent
 * - Determine the scan list
 * @return self
 */
  virtual Research &init() = 0;

// get file content, give it to supported Scan
  virtual std::string get_file_content(const std::string &path) = 0;

  Research &run() {
    std::vector<std::thread> workers;
    uint32_t max_worker_num = get_max_concurrent();
    for (const auto &path : m_support_file_paths) {
      workers.emplace_back([&]() {
        std::unique_lock worker_lock{m_worker_run_mtx};

        m_worker_run_cv.wait(worker_lock,
                             [&] { return m_worker_num < max_worker_num; });
        ++m_worker_num;
        worker_lock.unlock();

        Scan *s = create_scan(path);
        s->init(get_file_content(path));
        std::vector<TaskResult> res{s->run()};
        {
          std::lock_guard lock{m_result_mtx};
          m_results.insert(m_results.end(),
                           std::make_move_iterator(res.begin()),
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

// overwrite as default
  void export_result() {
    std::ofstream fs{};
    fs.open(m_out_path, std::ios_base::out);
    std::lock_guard lock(m_result_mtx);
    size_t result_n = m_results.size();
    try {
      fs << '[';
      for (int i = 0; i < result_n; i++) {
        TaskResult &result = m_results.at(i);
        fs << format_task_result(result.task_id, result.result, result.file_type);
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

  static bool is_supported_type(const std::string &file_path) {
    return File::get_filetype(file_path) != FileType::Unknown;
  }
};
}  // namespace iiran

#endif  // JEAN_RESEARCH_H
