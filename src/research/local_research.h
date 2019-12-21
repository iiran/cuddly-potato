//
// Created by yiran feng on 2019/12/18.
//

#ifndef JEAN_LOCAL_RESEARCH_H
#define JEAN_LOCAL_RESEARCH_H

#include "../research.h"

namespace iiran {

class LocalResearch : public Research {
 public:
  explicit LocalResearch(std::string out_path)
	  : Research(std::move(out_path)) {};

  LocalResearch &add_target(std::string target) override {
	m_root_dir = std::move(target);
	return *this;
  };

  std::string get_file_content(const std::string &path) override {
	if (path.length() == 0) throw std::logic_error("file path not set");

	std::ifstream m_fs{};
	m_fs.open(path, std::fstream::in | std::fstream::ate);
	auto f_size = m_fs.tellg();
	if (f_size > Research::FILE_MAX_SIZE) {
	  if (m_fs.is_open()) m_fs.close();
	  throw std::out_of_range("failed to read file");  // too large or not exist
	}

	m_fs.seekg(0);
	std::string text = {std::istreambuf_iterator<char>{m_fs},
						std::istreambuf_iterator<char>{}};
	if (m_fs.is_open()) m_fs.close();
	return text;
  }

  LocalResearch &init() override {
	if (m_root_dir.length() == 0) throw std::logic_error("root path not set");

	std::vector<std::string> supported_file_paths{};
	get_all_supported_local_recursive(m_root_dir, supported_file_paths);
	m_support_file_paths = std::move(supported_file_paths);
	return *this;
  }

 private:
  std::string m_root_dir;

  static void get_all_supported_local_recursive(
	  const std::string &path, std::vector<std::string> &v_path) {
	for (const auto &entry : std::filesystem::directory_iterator(path)) {
	  std::string entry_path = entry.path();
	  if (entry.is_directory()) {
		get_all_supported_local_recursive(entry_path, v_path);
	  } else {
		if (Research::is_supported_type(entry_path)) {
		  v_path.push_back(entry_path);
		}
	  }
	}
  }
};

}  // namespace iiran

#endif  // JEAN_LOCAL_RESEARCH_H
