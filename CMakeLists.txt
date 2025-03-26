cmake_minimum_required(VERSION 3.15)
project(MPointersProject)

set(CMAKE_CXX_STANDARD 17)

# Si instalaste gRPC manualmente, dile a CMake dónde buscarlo
set(CMAKE_PREFIX_PATH "/usr/local")

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

include_directories(
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

link_directories(/usr/local/lib)

# Encuentra las bibliotecas necesarias de Abseil (solo las que tienes)
foreach(lib
    absl_base
    absl_int128
    absl_strings
    absl_strings_internal
    absl_symbolize
    absl_stacktrace
    absl_debugging_internal
    absl_demangle_internal
    absl_throw_delegate
    absl_time
    absl_civil_time
    absl_time_zone
    absl_synchronization
    absl_malloc_internal
    absl_log_severity
    absl_log_internal_check_op
    absl_flags
    absl_flags_commandlineflag
    absl_flags_commandlineflag_internal
    absl_flags_config
    absl_flags_internal
    absl_flags_marshalling
    absl_flags_program_name
    absl_flags_reflection
    absl_flags_private_handle_accessor
    absl_raw_logging_internal
    absl_str_format_internal
)
    find_library(${lib}_LIB NAMES ${lib} PATHS /usr/local/lib NO_DEFAULT_PATH)
    if(${lib}_LIB)
        list(APPEND ABSEIL_LIBRARIES ${${lib}_LIB})
    endif()
endforeach()

add_executable(memory_manager
    src/main.cpp
    src/memory_manager.cpp
    src/grpc_server.cpp
    generated/memory_manager.pb.cc
    generated/memory_manager.grpc.pb.cc
)

target_link_libraries(memory_manager
    gRPC::grpc++
    protobuf::libprotobuf
    ${ABSEIL_LIBRARIES}
)
