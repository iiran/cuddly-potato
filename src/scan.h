//
// Created by yiran feng on 2019/12/9.
//

#ifndef JEAN_SCAN_H
#define JEAN_SCAN_H

#include <algorithm>
#include <atomic>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "task.h"

class Task;

namespace iiran {
std::string format_task_result(int32_t task_id, const std::string &task_res,
                               const std::string &file_path);

class Scan {
 public:
  explicit Scan() = default;

  explicit Scan(const std::vector<Task *> &tasks) {
    for (const auto &pt : tasks) {
      m_tasks.emplace_back(std::unique_ptr<Task>(pt));
    }
  }

  void init(std::string content) { m_text = std::move(content); };

  std::vector<std::string> run() {
    unsigned max_worker_num = std::thread::hardware_concurrency();

    std::vector<std::thread> workers;

    for (const auto &t : m_tasks) {
      workers.emplace_back([&]() {
        std::unique_lock worker_lock{m_worker_run_mtx};
        m_worker_run_cv.wait(worker_lock,
                             [&] { return m_worker_num < max_worker_num; });
        ++m_worker_num;
        worker_lock.unlock();
        std::string task_res = t->operator()(m_text);
        std::string format_res =
            format_task_result(t->get_id(), task_res, m_file_path);
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
  explicit CppScan() : Scan({
                                new CountLine(),
                                new CountSemicolon(),
                                new CountCCommentLine(),
                                new CountBlankLine(),
                                new CountCodeLine<CountCCommentLine>(),
                                new VariableNameStatistic(),
                                new GetIndentStyle(),
                            }) {}

  explicit CppScan(std::string file_path) : CppScan() {
    m_file_path = std::move(file_path);
  }
};

class PythonScan : public Scan {
 public:
  explicit PythonScan() : Scan({new CountLine()}) {}

  explicit PythonScan(std::string file_path) : PythonScan() {
    m_file_path = std::move(file_path);
  }
};

class TextScan : public Scan {
 public:
  explicit TextScan() : Scan({new CountLine()}) {}

  explicit TextScan(std::string filepath) { m_file_path = std::move(filepath); }
};

Scan *create_scan(std::string filePath);

}  // namespace iiran

#endif  // JEAN_SCAN_H
