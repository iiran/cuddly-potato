//
// Created by yiran feng on 2019/12/12.
//

#include <stdexcept>
#include <deque>
#include "argo.h"

namespace iiran {

    Argo::Argo(int argc, char **argv) {
        if (argc == 0) throw std::logic_error("argc should greater than 0");
        if (argv == nullptr) throw std::logic_error("argv is nullptr");

        std::deque<std::string> v_argv(argc);
        for (int i = 0; i < argc; i++) {
            v_argv.emplace_back(argv[i]);
        }

        set_args(std::move(v_argv));
    }

    std::string Argo::get_value(const std::string &key) {
        try {
            std::vector<std::string> &v = m_arg_map.at(key);
            return v.at(0);
        } catch (std::exception &e) {
            return "";
        }
    }

    bool Argo::has_value(const std::string &key) {
        try {
            std::vector<std::string> &v = m_arg_map.at(key);
            return true;
        } catch (std::exception &e) {
            return false;
        }
    }

    std::vector<std::string> Argo::get_array(const std::string &key) {
        try {
            std::vector<std::string> &v = m_arg_map.at(key);
            return v;
        } catch (std::exception &v) {
            return {};
        }
    }

    void Argo::set_args(std::deque<std::string> &&v_argv) {
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

    Argo::Argo(std::deque<std::string> &&v_argv) {
        set_args(std::move(v_argv));
    }
}