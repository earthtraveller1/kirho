include(CTest)

add_executable(result result.cpp)
add_test(NAME result COMMAND result)
target_link_libraries(result PRIVATE kirho)
