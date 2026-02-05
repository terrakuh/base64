cmake_minimum_required(VERSION 3.23)
project("base64"
        LANGUAGES CXX)

include(GNUInstallDirs)
add_library(base64 INTERFACE)
target_sources(
        base64
        PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        FILES
        base64.hpp
)
target_compile_features(base64 INTERFACE cxx_std_11)
add_library(base64::base64 ALIAS base64)

option(BUILD_MAIN "Build main.cpp for testing" OFF)
if (BUILD_MAIN)
    add_executable(main "main.cpp")
    target_link_libraries(main PRIVATE base64)
endif ()

install(TARGETS base64 EXPORT base64Targets FILE_SET public_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/base64)
install(EXPORT base64Targets NAMESPACE base64:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/base64 FILE base64Config.cmake)
include(CMakePackageConfigHelpers)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/base64Config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/base64
)