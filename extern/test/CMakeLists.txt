# cmake_minimum_required(VERSION 3.21)
# project(test)

# add_executable(test test.cpp)

# target_link_libraries(test PRIVATE Python3::Python pybind11::embed )


# # Copy Python DLL to the build folder if different
# add_custom_command(
#   TARGET test
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND}
#   -E copy_if_different ${CPYTHON_BIN} $<TARGET_FILE_DIR:test>
# )

# # Copy the Python stdlib into the build folder (needed by the embedded python)
# add_custom_command(
#   TARGET ${PROJECT_NAME} 
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND} 
#   -E copy_directory ${CPYTHON_STDLIB_DIR} $<TARGET_FILE_DIR:test>/lib
# )