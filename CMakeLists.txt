cmake_minimum_required(VERSION 3.16)
project(basicpp VERSION 0.0.1 LANGUAGES CXX)

option(BASICPP_ENABLE_TESTS "Enable basicpp tests" ON)

add_library(basicpp INTERFACE)
target_compile_features(basicpp INTERFACE cxx_std_20)
target_include_directories(
    basicpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(basicpp INTERFACE BASICPP_VERSION="${PROJECT_VERSION}")

add_library(basicpp_frontend STATIC
    src/frontend/lexer.cpp
    src/frontend/parser.cpp
)
target_compile_features(basicpp_frontend PUBLIC cxx_std_20)
target_link_libraries(basicpp_frontend PUBLIC basicpp)
target_include_directories(basicpp_frontend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(BASICPP_ENABLE_TESTS)
    enable_testing()
    file(GLOB BASICPP_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
    foreach(test_src ${BASICPP_TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE basicpp_frontend basicpp)
        target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()

add_subdirectory(src/cli)
