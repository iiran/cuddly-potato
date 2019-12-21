//
// Created by yiran feng on 2019/12/12.
//

#ifndef JEAN_ARGO_H
#define JEAN_ARGO_H

#include <map>
#include <string>
#include <vector>

/**
 *                === TYPE 1 ===
 * [OK]  -x -v                    ->  "-x":[], "-v":[]
 * [OK]  -x a -v                  ->  "-x":["a"], "-v":[]
 * [OK]  --xx a b c --yy          ->  "--x":["a", "b", "c"], "--yy":[]
 * [OK]  --s '  little words  '   ->  "--s":["  little words  "]
 * [OK]  --s  ' a ' ' b '         ->  "--s":[" a ", " b "]
 */

namespace iiran {

class Argo final {
 public:
  explicit Argo(int argc, char *argv[]) {
      if (argc == 0) throw std::logic_error("argc should greater than 0");
      if (argv == nullptr) throw std::logic_error("argv is nullptr");

      std::deque<std::string> v_argv(argc);
      for (int i = 0; i < argc; i++) {
          v_argv.emplace_back(argv[i]);
      }

      set_args(std::move(v_argv));
  }

  explicit Argo(std::deque<std::string> &&v_argv) {
      set_args(std::move(v_argv));
  }

  explicit Argo() = default;

  void set_args(std::deque<std::string> &&v_argv) {
      m_exec_path = v_argv[0];
      v_argv.pop_front();

      std::string key{};
      for (const auto &s : v_argv) {
          if (s.find('-') == 0) /* is key */ {
              key = s;
              m_arg_map[key] = {};
          } else /* is value */ {
              m_arg_map[key].push_back(s);
          }
      }
  }

  bool has_value(const std::string &key) {
      try {
          std::vector<std::string> &v = m_arg_map.at(key);
          return true;
      } catch (std::exception &e) {
          return false;
      }
  }

  std::string get_value(const std::string &key) {
      try {
          std::vector<std::string> &v = m_arg_map.at(key);
          return v.at(0);
      } catch (std::exception &e) {
          return "";
      }
  }

  std::vector<std::string> get_array(const std::string &key) {
      try {
          std::vector<std::string> &v = m_arg_map.at(key);
          return v;
      } catch (std::exception &v) {
          return {};
      }
  }

 private:
  std::string m_exec_path;
  std::map<std::string, std::vector<std::string>> m_arg_map;
};
}  // namespace iiran

#endif  // JEAN_ARGO_H
