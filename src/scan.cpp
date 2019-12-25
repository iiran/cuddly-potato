

#include "filetype.h"
#include "scan.h"
#include "task.h"
#include <map>
//
// Created by yiran feng on 2019/12/9.
//

namespace iiran {

Scan *create_scan(std::string file_path) {
  FileType ft = File::get_filetype(file_path);
  switch (ft) {
    case FileType::Cpp:return new CppScan(std::move(file_path));
    case FileType::Python:return new PythonScan(std::move(file_path));
    default:return new TextScan(std::move(file_path));
      // throw std::invalid_argument("unsupported type");
  }
}

// json
std::string format_task_result(int32_t task_id, const std::string &task_res,
                               int32_t file_type) {
  char buf[Task::TASK_LINE_MAX]{0};
  assert(task_res.length() < Task::TASK_LINE_MAX);
  std::snprintf(buf, sizeof buf, R"({"task_id":%u,"res":"%s","file_type":"%d"})", task_id,
                task_res.c_str(), file_type);
  return std::string{buf};
}

}  // namespace iiran
