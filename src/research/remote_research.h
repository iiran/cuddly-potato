//
// Created by yiran feng on 2019/12/18.
//

#ifndef JEAN_REMOTE_RESEARCH_H
#define JEAN_REMOTE_RESEARCH_H

#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <thread>
#include "../research.h"

namespace iiran {

const static char *DefaultUserAgent =
	"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:6.0) Gecko/20100101 Firefox/6.0";
const static uint32_t DefaultThreadMultiRatio = 2;

struct RequestOption {
  std::string UserAgent{};
};

class RemoteResearch : public Research {
 public:
  RemoteResearch &add_target(std::string target) override {
	m_root_url = std::move(target);
	return *this;
  }

  uint32_t get_max_concurrent() override {
	return std::thread::hardware_concurrency() * 2;
  }

  explicit RemoteResearch(std::string out_path)
	  : Research(std::move(out_path)) {};

  virtual std::vector<std::string> get_all_files() = 0;

  RemoteResearch &init() override {
	std::vector<std::string> files{std::move(get_all_files())};
	for (auto &f : files) {
	  if (is_supported_type(f)) {
		m_support_file_paths.emplace_back(f);
	  }
	}
	return *this;
  }

 protected:
  std::string m_root_url{};

  static nlohmann::json request_json(const std::string &url) {
	std::string json_res{};
	try {
	  curlpp::Easy request;
	  using namespace curlpp::Options;
	  // request.setOpt(Verbose(true));
	  request.setOpt(Url(url));
	  request.setOpt(UserAgent(DefaultUserAgent));
	  std::ostringstream os;
	  curlpp::options::WriteStream ws(&os);
	  request.setOpt(ws);
	  request.perform();
	  json_res = os.str();
	} catch (curlpp::LogicError &e) {
	  std::cout << e.what() << std::endl;
	} catch (curlpp::RuntimeError &e) {
	  std::cout << e.what() << std::endl;
	}
	return nlohmann::json::parse(json_res);
  }

 private:
  RequestOption m_req_opt{DefaultUserAgent};
};

}  // namespace iiran

#endif  // JEAN_REMOTE_RESEARCH_H
