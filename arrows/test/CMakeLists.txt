set(plugin_test_headers
    test_headers.h
)

kwiver_install_headers(
    SUBDIR arrows/test 
    ${plugin_test_headers}
)

kwiver_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_test_export.h
    NOPATH SUBDIR arrow/test
)

set(
    plugin_test_sources
    test_headers.cpp
)

kwiver_add_library(
    kwiver_algo_test
    ${plugin_test_headers}
    ${plugin_test_sources}
)

target_include_directories(
    kwiver_algo_test PUBLIC
    /usr/local/include/colmap
    /usr/local/include/eigen3
)

target_link_directories(
    kwiver_algo_test PUBLIC
    /usr/local/lib/colmap
)

target_link_libraries(
    kwiver_algo_test
    PUBLIC vital
    colmap
)

