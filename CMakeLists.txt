cmake_minimum_required(VERSION 3.10)
project(MPCStreaming)

# Đặt C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm libcurl
find_package(CURL REQUIRED)

# Thêm executable
add_executable(mpc_streaming src/code_algorithm_mpc_libcurl.cpp)

# Link libcurl
target_link_libraries(mpc_streaming PRIVATE CURL::libcurl)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mpc_streaming PRIVATE -Wall -Wextra -O2)
endif()

# Print thông tin
message(STATUS "CURL include dir: ${CURL_INCLUDE_DIRS}")
message(STATUS "CURL libraries: ${CURL_LIBRARIES}")

# ============================================================================
# Custom targets để tiện sử dụng
# ============================================================================

# Target để chạy chương trình
add_custom_target(run
    COMMAND mpc_streaming
    DEPENDS mpc_streaming
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running MPC streaming client..."
)

# Target để clean các thư mục download và decoded
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/download
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/decoded_output
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/download
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/decoded_output
    COMMENT "Cleaning download and decoded_output directories..."
)

# Target để copy scripts
add_custom_target(install-scripts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_SOURCE_DIR}/scripts/auto_decode.sh 
        ${CMAKE_SOURCE_DIR}/auto_decode.sh
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_SOURCE_DIR}/scripts/batch_decode_all.sh 
        ${CMAKE_SOURCE_DIR}/batch_decode_all.sh
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/auto_decode.sh
    COMMAND chmod +x ${CMAKE_SOURCE_DIR}/batch_decode_all.sh
    COMMENT "Installing and setting permissions for decode scripts..."
)

# Target để setup toàn bộ môi trường
add_custom_target(setup
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/download
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/decoded_output
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/scripts
    DEPENDS install-scripts
    COMMENT "Setting up project directories and scripts..."
)

# Hiển thị hướng dẫn sau khi build
add_custom_command(TARGET mpc_streaming POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMAND ${CMAKE_COMMAND} -E echo "To run: ./mpc_streaming"
    COMMAND ${CMAKE_COMMAND} -E echo "Or use: make run"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Make sure to:"
    COMMAND ${CMAKE_COMMAND} -E echo "1. Update paths in src/code_algorithm_mpc_libcurl.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo "2. Update paths in auto_decode.sh"
    COMMAND ${CMAKE_COMMAND} -E echo "3. bash ./auto_decode.sh ./download/segment_003_HIGH.bin"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
)