# CMakeLists.txt


# [[file:README.org::*CMakeLists.txt][CMakeLists.txt:1]]
cmake_minimum_required(VERSION 3.4.3)
project(slang)

find_package(LLVM REQUIRED)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(slang main.c)

llvm_map_components_to_libnames(llvm_libs support core irreader executionengine interpreter mc nativecodegen)

target_link_libraries(slang ${llvm_libs})
# CMakeLists.txt:1 ends here
