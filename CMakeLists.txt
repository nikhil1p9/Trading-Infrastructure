cmake_minimum_required(VERSION 3.10)
project(DeribitTradingSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# Classic Boost find
# -------------------------------

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    include
)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(main ${SOURCES})

target_link_libraries(main
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    pthread
    CURL::libcurl
)
