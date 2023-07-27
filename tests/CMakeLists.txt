include(CTest)

add_executable(result result.cpp)
add_test(NAME result COMMAND result)
target_include_directories(result PRIVATE "${CMAKE_SOURCE_DIR}/include")
