#include <filesystem>
#include <memory>
#include "argo.h"
#include "research.h"
#include "research/github_research.h"
#include "research/local_research.h"
#include "scan.h"

std::unique_ptr<iiran::Research> create_research(
        const std::string &target_path, const std::string &output_path) {
    std::unique_ptr<iiran::Research> rsh;
    if (target_path.find_first_of('/') == 0 ||
            target_path.find_first_of('.') == 0) {
        rsh = std::make_unique<iiran::LocalResearch>(output_path);
    } else if (target_path.find_first_of("github.com") == 0) {
        rsh = std::make_unique<iiran::GitHubResearch>(output_path);
    } else {
        throw std::invalid_argument("unknown target");
    }
    rsh->add_target(target_path);
    return rsh;
}

int main(int argc, char *argv[]) {
    const char *help = R"(-h --help)";

    iiran::Argo arg(argc, argv);

    if (arg.has_value("--help")) {
        std::cout << help << std::endl;
        exit(0);
    }

    std::string target{std::filesystem::current_path().string()};
    if (arg.has_value("--target")) {
        target = arg.get_value("--target");
    }
    std::string out{};
    if (arg.has_value("--out")) {
        out = arg.get_value("--out");
    }

    auto rsh = create_research(target, out);
    rsh->init().run();

    if (!out.empty()) {
        rsh->export_result();
    }

    return 0;
}
