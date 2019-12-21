cmake_minimum_required(VERSION 3.15)
project(jean)
set(CMAKE_CXX_STANDARD 17)

set(CURLPP_PREFIX /usr/local/Cellar/curlpp/0.8.1)
set(USR_PREFIX /usr)

include_directories(${CURLPP_PREFIX}/include)
include_directories(/Users/yiranfeng/repo/nlohmann-json/json/include)
FIND_PACKAGE(Boost 1.71 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_executable(jean src/main.cpp src/scan.cpp src/scan.h src/task.cpp src/task.h src/argo.h src/research.h src/filetype.h src/filetype.h src/research/remote_research.h src/research/local_research.h src/research/github_research.h)

target_link_libraries(jean ${CURLPP_PREFIX}/lib/libcurlpp.a ${USR_PREFIX}/lib/libcurl.dylib)
TARGET_LINK_LIBRARIES(jean LINK_PUBLIC ${Boost_LIBRARIES})
