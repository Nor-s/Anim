cmake_minimum_required(VERSION 3.1)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

project (PythonEmbedded)

set (CMAKE_CXX_STANDARD 17)


# Specify build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")



# https://github.com/matusnovak/python-embedded-example-project/blob/master/CMakeLists.txt
# https://github.com/caiorss/example-pybind11-vcpkg
# https://github.com/pybind/pybind11/issues/3403
# https://github.com/blender/blender/blob/594f47ecd2d5367ca936cf6fc6ec8168c2b360d0/build_files/build_environment/cmake/python.cmake
# find_package(pybind11 CONFIG REQUIRED)

# Add the cpython as an external project that will be included in the build
if(MSVC)
  if(CMAKE_CL_64)
    set(CPYTHON_PLATFORM x64)
    set(CPYTHON_BUILD_DIR amd64)
  else()
    set(CPYTHON_PLATFORM x86)
    set(CPYTHON_BUILD_DIR win32)
  endif()

  set(CPYTHON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cpython/)
  set(CPYTHON_BINARY_INTERNAL ${CPYTHON_SRC}/PCBuild/${CPYTHON_BUILD_DIR}/python.exe)
  set(CPYTHON_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/python)
  set(CPYTHON_BINARY ${CPYTHON_OUTPUT_DIR}/python.exe)
  set(CPYTHON_PIP ${CPYTHON_OUTPUT_DIR}/Scripts/pip.exe)

  ExternalProject_Add(CPYTHON
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CPYTHON_SRC}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cd ${CPYTHON_SRC}/PCbuild/ && 
                  call build.bat -e -p ${CPYTHON_PLATFORM}
    UPDATE_COMMAND "" 
    INSTALL_COMMAND ${CPYTHON_BINARY_INTERNAL} 
                                  ${CPYTHON_SRC}/PC/layout/main.py 
                                  -b ${CPYTHON_SRC}/PCbuild/${CPYTHON_BUILD_DIR}
                                  -s ${CPYTHON_SRC} 
                                  -t ${CPYTHON_SRC}/temp/  
                                  --preset-default
                                  --copy ${CPYTHON_OUTPUT_DIR} &&
                     curl https://bootstrap.pypa.io/get-pip.py -o ${CPYTHON_OUTPUT_DIR}/get-pip.py &&
                     ${CPYTHON_BINARY} ${CPYTHON_OUTPUT_DIR}/get-pip.py &&
                     ${CPYTHON_PIP} install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt

  )
else()
  # ExternalProject_Add(CPYTHON
  #   DOWNLOAD_COMMAND ""
  #   SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/cpython
  #   CONFIGURE_COMMAND cd ${CMAKE_SOURCE_DIR}/libs/cpython && ./configure --disable-static --enable-shared 
  #   BUILD_COMMAND cd ${CMAKE_SOURCE_DIR}/libs/cpython && make
  #   INSTALL_COMMAND ""
  #   TEST_COMMAND ""
  # )
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CPYTHON_OUTPUT_DIR} ${EXECUTABLE_OUTPUT_PATH}/python)

set(CPYTHON_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/python)
set(CPYTHON_BINARY ${CPYTHON_OUTPUT_DIR}/python.exe)
set(CPYTHON_PIP ${CPYTHON_OUTPUT_DIR}/Scripts/pip.exe)



set(CPYTHON_STDLIB_DIR ${CPYTHON_OUTPUT_DIR}/Lib)
set(CPYTHON_LIB_DIR ${CPYTHON_OUTPUT_DIR}/libs)
set(PYBIND_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11/include)
set(CPYTHON_INCLUDE_DIR ${CPYTHON_OUTPUT_DIR}/include)


ExternalProject_Add(PYBIND
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11
  CMAKE_ARGS -DPYBIND11_TEST=OFF 
             -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
             -DPYTHONLIBS_FOUND=ON 
             -DPYTHON_MODULE_EXTENSION=.lib 
             -DPYTHON_EXECUTABLE=${CPYTHON_BINARY}
             -DPYTHON_LIBRARY=${CPYTHON_OUTPUT_DIR}/libs
             -DPYTHON_INCLUDE_DIR=${CPYTHON_INCLUDE_DIR}
  BUILD_COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE}
  UPDATE_COMMAND "" 
  INSTALL_COMMAND ""
  TEST_COMMAND ""
)

message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS} ${pybind11_DEFINITIONS} ${pybind11_LIBRARIES}")

link_directories(${CPYTHON_OUTPUT_DIR})

add_executable(${PROJECT_NAME}  test/main.cpp )
# Add macros
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX=1)

add_dependencies(${PROJECT_NAME}  PYBIND) # pybind11::embed)
add_dependencies(${PROJECT_NAME}  CPYTHON)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CPYTHON_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PYBIND_INCLUDE_DIR})

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${CPYTHON_OUTPUT_DIR}/libs/python310.lib)
# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

target_compile_options(${PROJECT_NAME} PRIVATE ${CPYTHON_DEFINITIONS})

# # Copy Python DLL to the build folder if different
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_if_different ${CPYTHON_OUTPUT_DIR}/python310.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_if_different ${CPYTHON_OUTPUT_DIR}/python3.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
# Copy our python sources to the build folder
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/app $<TARGET_FILE_DIR:${PROJECT_NAME}>/app
)

# # Copy the Python stdlib into the build folder (needed by the embedded python)
# add_custom_command(
#   TARGET ${PROJECT_NAME} 
#   POST_BUILD
#   COMMAND ${CMAKE_COMMAND} 
#   -E copy_directory ${CPYTHON_STDLIB_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib
# )
