find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_executable(nature-of-code main.cpp)

target_link_libraries(nature-of-code
  PRIVATE
  fmt::fmt-header-only
  spdlog::spdlog_header_only)
