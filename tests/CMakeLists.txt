include(CTest)

add_executable(result result.cpp)
add_test(NAME result COMMAND result)
target_link_libraries(result PRIVATE kirho)

add_executable(error-handler error-handler.cpp)
add_test(NAME error-handler COMMAND error-handler)
target_link_libraries(error-handler PRIVATE kirho)
