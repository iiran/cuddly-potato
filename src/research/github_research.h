//
// Created by yiran feng on 2019/12/19.
//

#ifndef JEAN_GITHUB_RESEARCH_H
#define JEAN_GITHUB_RESEARCH_H

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <curlpp/Easy.hpp>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include "../filetype.h"
#include "remote_research.h"

namespace iiran {
namespace Github {
static const std::string GitHubAPIBase{"api.github.com"};
static const std::string GitHubDomain{"github.com"};
}  // namespace Github
static const std::string Protocol{"https"};

class GitHubResearch : public RemoteResearch {
 public:
  explicit GitHubResearch(std::string out_path)
          : RemoteResearch{std::move(out_path)} {}

  std::vector<std::string> get_all_files() override {
      std::pair<std::string, std::string> orgAndRepo{
              std::move(get_org_and_repo(m_root_url))};
      m_file_url_link =
              std::move(list_repo_file(orgAndRepo.first, orgAndRepo.second));
      std::vector<std::string> files{};
      for (const auto &link : m_file_url_link) {
          files.emplace_back(link.first);
      }
      return files;
  }

  std::string get_file_content(const std::string &path) override {
      const std::string content_request_url = m_file_url_link.at(path);
      auto res{std::move(request_json(content_request_url))};

      std::array<char, 10'000> arr{};
      std::string raw_content{std::move(res["content"].get<std::string>())};
      boost::replace_all(raw_content, "\n", "");
      const std::pair<std::size_t, std::size_t> pair =
              boost::beast::detail::base64::decode(arr.data(), raw_content.c_str(),
                                                   raw_content.size());
      size_t out_size = pair.first;
      return std::string{std::begin(arr), std::begin(arr) + out_size};
  }

 private:
  std::map<std::string, std::string> m_file_url_link;

  static std::pair<std::string, std::string> get_org_and_repo(
          const std::string &url) {
      std::vector<std::string> sv{};
      boost::split(sv, url, boost::is_any_of("/"));
      std::pair<std::string, std::string> res_pair{};
      for (int i = 0, sv_size = sv.size(); i < sv_size; ++i) {
          if (sv[i] == std::string_view(Github::GitHubDomain)) {
              if (i + 2 < sv_size) {
                  res_pair.first = sv[i + 1];
                  res_pair.second = sv[i + 2];
              }
          }
      }
      return res_pair;
  }

  static std::string format_github_request_url(const std::string &s) {
      static std::string url_left{Protocol + "://" + Github::GitHubAPIBase + '/'};
      return url_left + s;
  }

  static std::map<std::string, std::string> list_repo_file(
          const std::string &org, const std::string &repo,
          const std::string &base = "") {
      static std::string repo_api_prefix = "repos/";
      std::string url{repo_api_prefix + org + '/' + repo + "/contents/"};
      std::map<std::string, std::string> file_url_map{};
      try {
          list_repo_file_recursive(format_github_request_url(url), file_url_map,
                                   base);
      } catch (std::exception &e) {
          std::cout << e.what() << std::endl;
      }
      return file_url_map;
  }

  static void list_repo_file_recursive(
          const std::string &url, std::map<std::string, std::string> &file_url_link,
          const std::string &curr_path) {
      static const char type_file_pre = std::string{"file"}[0];
      static const char type_dir_pre = std::string{"dir"}[0];
      auto res{RemoteResearch::request_json(url + curr_path)};
      if (res.is_object()) {
          // is ban ?
          if (res.find("documentation_url") != res.end()) {
              std::string help{
                      std::move(res["documentation_url"].get<std::string>())};
              if (help.find_first_of("#rate-limiting") != std::string::npos) {
                  throw std::logic_error("request limited");
              }
          }
          // unknown
          throw std::logic_error("unknown error");
      } else if (res.is_array()) {
          for (auto &e : res) {
              try {
                  const std::string &e_path = e["path"];
                  const char e_type_pre = static_cast<std::string>(e["type"])[0];
                  if (e_type_pre == type_file_pre) {
                      file_url_link[e_path] = url + e_path;
                      std::cout << e_path << std::endl;
                  } else if (e_type_pre == type_dir_pre) {
                      list_repo_file_recursive(url, file_url_link, e_path);
                  } else {
                      throw std::logic_error("unreachable: unknown file type");
                  }
              } catch (nlohmann::json::exception &e) {
                  std::cout << e.what() << std::endl;
              }
          }
      }
  }
};

}  // namespace iiran

#endif  // JEAN_GITHUB_RESEARCH_H
