cmake_minimum_required(VERSION 3.20)
project(Brainfuck2D)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Antlr ---------------------------------------------------------
add_definitions(-DANTLR4CPP_STATIC)

include(ExternalAntlr4Cpp)

message("Including antlr dirs: ${ANTLR4_INCLUDE_DIRS}")
include_directories(${ANTLR4_INCLUDE_DIRS})

set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr-4.13.2-complete.jar)

find_package(ANTLR REQUIRED)

antlr_target(Sugar ${CMAKE_CURRENT_SOURCE_DIR}/antlr/Sugar.g4 VISITOR)

include_directories(${ANTLR_Sugar_OUTPUT_DIR})
# Antlr ---------------------------------------------------------


# LLVM ----------------------------------------------------------
message("Searching for LLVM.")

find_package(LLVM REQUIRED CONFIG)
    
message("Found LLVM ${LLVM_PACKAGE_VERSION}")
message("Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs core support)
message("Adding LLVM-Libs: ${llvm_libs}")
# LLVM ----------------------------------------------------------

file(GLOB_RECURSE src_files
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${src_files} ${ANTLR_Sugar_CXX_OUTPUTS})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE antlr4_static ${llvm_libs})
