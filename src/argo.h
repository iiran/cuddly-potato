//
// Created by yiran feng on 2019/12/12.
//

#ifndef CUDDLY_POTATO_ARGO_H
#define CUDDLY_POTATO_ARGO_H

#include <string>
#include <vector>
#include <map>

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
        explicit Argo(int argc, char *argv[]);

        explicit Argo(std::deque<std::string> &&v_argv);

        explicit Argo() = default;

        void set_args(std::deque<std::string> &&v_argv);

        bool has_value(const std::string &key);

        std::string get_value(const std::string &key);

        std::vector<std::string> get_array(const std::string &key);

    private:
        std::string m_exec_path;
        std::map<std::string, std::vector<std::string>> m_arg_map;
    };
}


#endif //CUDDLY_POTATO_ARGO_H
