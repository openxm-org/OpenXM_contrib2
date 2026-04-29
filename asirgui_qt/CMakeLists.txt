cmake_minimum_required(VERSION 3.22)

project(AsirGUI VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(CTest)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets Test)

get_filename_component(WORKSPACE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(OPENXM_HOME_DEFAULT "${WORKSPACE_ROOT}/OpenXM")
set(OPENXM_HOME "${OPENXM_HOME_DEFAULT}" CACHE PATH "Path to OpenXM")
set(OPENXM_LIBDIR "${OPENXM_HOME}/lib")
set(OPENXM_INCDIR "${OPENXM_HOME}/include")

find_library(LIBASIR asir PATHS "${OPENXM_LIBDIR}" REQUIRED NO_DEFAULT_PATH)
find_library(LIBGC gc PATHS "${OPENXM_LIBDIR}" REQUIRED NO_DEFAULT_PATH)
find_library(LIBMPFR mpfr PATHS "${OPENXM_LIBDIR}" REQUIRED)
find_library(LIBMPC mpc PATHS "${OPENXM_LIBDIR}" REQUIRED)
find_library(LIBGMP gmp PATHS "${OPENXM_LIBDIR}" REQUIRED)

add_library(asirgui_core STATIC
    core/AsirEngine.h
    core/AppConfig.h
    core/AppConfig.cpp
    core/CommandHistory.h
    core/CommandHistory.cpp
    core/LocalAsirEngine.h
    core/LocalAsirEngine.cpp
)

target_include_directories(asirgui_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${OPENXM_INCDIR}"
)

target_link_libraries(asirgui_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        "${LIBASIR}"
        "${LIBGC}"
        "${LIBMPFR}"
        "${LIBMPC}"
        "${LIBGMP}"
        pthread
        m
)

add_executable(asirgui
    desktop/linux/main.cpp
    desktop/common/InputWidget.cpp
    desktop/common/MainWindow.cpp
    desktop/common/MathView.cpp
    desktop/common/PlotView.cpp
    resources/resources.qrc
)

target_link_libraries(asirgui
    PRIVATE
        asirgui_core
        Qt6::Widgets
        Qt6::WebEngineWidgets
)

target_compile_definitions(asirgui
    PRIVATE
        ASIRGUI_DEFAULT_OPENXM_HOME="${OPENXM_HOME}"
)

if(BUILD_TESTING)
    add_executable(test_history tests/test_history.cpp)
    target_link_libraries(test_history PRIVATE asirgui_core)
    add_test(NAME test_history COMMAND test_history)

    add_executable(test_engine tests/test_engine.cpp)
    target_link_libraries(test_engine PRIVATE asirgui_core Qt6::Test)
    target_compile_definitions(test_engine
        PRIVATE
            ASIRGUI_DEFAULT_OPENXM_HOME="${OPENXM_HOME}"
    )
    add_test(NAME test_engine COMMAND test_engine)

    add_executable(test_input_widget
        tests/test_input_widget.cpp
        desktop/common/InputWidget.cpp
    )
    target_link_libraries(test_input_widget PRIVATE asirgui_core Qt6::Widgets Qt6::Test)
    add_test(NAME test_input_widget COMMAND test_input_widget)
    set_tests_properties(test_input_widget PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
