//
// Created by yiran feng on 2019/12/12.
//

#ifndef CUDDLY_POTATO_RESEARCH_H
#define CUDDLY_POTATO_RESEARCH_H

#include <vector>
#include <mutex>

namespace iiran {
    class Research {
    public:
        static const std::string::size_type FILE_MAX_SIZE = 100'000'000;

        Research() = default;

        explicit Research(std::string out_path);

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

        Research &run();

        // overwrite as default
        void export_result();

    protected:
        std::vector<std::string> m_support_file_paths;
        std::uint32_t m_max_concurrency;
    private:
        std::vector<std::string> m_results;
        std::mutex m_result_mtx;
        std::string m_out_path;
        std::atomic_uint8_t m_worker_num{0};
        std::mutex m_worker_run_mtx;
        std::condition_variable m_worker_run_cv;
    };

    class RemoteResearch : public Research {
    public:
        RemoteResearch &add_target(std::string target) override;

        explicit RemoteResearch(std::string out_path);

        RemoteResearch &init() override;

    private:
        std::string m_root_url;
    };

    void get_all_supported_local_recursive(const std::string &path, std::vector<std::string> &v_path);

    class LocalResearch : public Research {
    public:
        explicit LocalResearch(std::string out_path);

        LocalResearch &add_target(std::string target) override;

        std::string get_file_content(const std::string &path) override;

        LocalResearch &init() override;

    private:
        std::string m_root_dir;
    };

}


#endif //CUDDLY_POTATO_RESEARCH_H
