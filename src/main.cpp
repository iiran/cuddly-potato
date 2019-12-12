#include "scan.h"
#include "argo.h"
#include "research.h"
#include <filesystem>

std::string get_path_root(int argc, char *argv[]) {
    const std::string param_key_path{"-r"};

    std::string path;
    if (argc > 1) {
        iiran::Argo arg(argc, argv);
        if (!arg.has_value(param_key_path)) {
            throw std::logic_error("-r is missing");
        }
        path = arg.get_value(param_key_path);
        std::cout << "using params path: " << path << std::endl;
    } else {
        path = std::filesystem::current_path();
        std::cout << "using working path: " << path << std::endl;
    }
    return path;
}


int main(int argc, char *argv[]) {

    std::string root = get_path_root(argc, argv);

    iiran::Research *rsh = new iiran::LocalResearch("/Users/yiranfeng/repo/cuddly-potato/runtime/out.json");
    rsh->add_target(root)
            .init()
            .run()
            .export_result();
    return 0;
}
