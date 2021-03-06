cmake_minimum_required(VERSION 3.10)

project(REDasm)

set(QT_VERSION_REQ "5.9")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(DEBUG_STL_ITERATORS      "Enable iterator debugging in Debug mode (GCC only)" OFF)
option(ENABLE_ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)
option(ENABLE_THREAD_SANITIZER  "Enable Thread Sanitizer" OFF)

string(TIMESTAMP REDASM_BUILD_TIMESTAMP "%Y%m%d")
set(REDASM_GIT_VERSION "unknown")

if(UNIX)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH ".")
endif()

find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Git)

if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE REDASM_GIT_VERSION
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    message(STATUS "Git not found, setting version to ${REDASM_GIT_VERSION}")
endif()

set(REDASM_BUILD_VERSION "${REDASM_BUILD_TIMESTAMP}.${REDASM_GIT_VERSION}")
add_definitions(-DREDASM_VERSION="${REDASM_BUILD_VERSION}")


add_subdirectory(LibREDasm)
add_subdirectory(QHexView)

if((CMAKE_BUILD_TYPE MATCHES Debug) OR (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
    add_subdirectory(unittest)
endif()

QT5_WRAP_UI(UI_HDRS ${UI_FILES})

# Widgets
file(GLOB_RECURSE WIDGETS_HEADERS widgets/*.h)
file(GLOB_RECURSE WIDGETS_SOURCES widgets/*.cpp)
file(GLOB_RECURSE WIDGETS_UIS     widgets/*.ui)

# Dialogs
file(GLOB_RECURSE DIALOGS_HEADERS dialogs/*.h)
file(GLOB_RECURSE DIALOGS_SOURCES dialogs/*.cpp)
file(GLOB_RECURSE DIALOGS_UIS     dialogs/*.ui)

# Models
file(GLOB_RECURSE MODELS_HEADERS models/*.h)
file(GLOB_RECURSE MODELS_SOURCES models/*.cpp)

# Renderer
file(GLOB_RECURSE RENDERER_HEADERS renderer/*.h)
file(GLOB_RECURSE RENDERER_SOURCES renderer/*.cpp)

# UI
file(GLOB_RECURSE UI_HEADERS ui/*.h)
file(GLOB_RECURSE UI_SOURCES ui/*.cpp)
file(GLOB_RECURSE UI_UIS ui/*.ui)

SET(HEADERS
    ${QHEXVIEW_HEADERS}
    ${REDASM_TEST_HEADERS}
    ${WIDGETS_HEADERS}
    ${DIALOGS_HEADERS}
    ${MODELS_HEADERS}
    ${RENDERER_HEADERS}
    ${UI_HEADERS}
    mainwindow.h
    themeprovider.h
    redasmsettings.h
    disassembleractions.h)

SET(SOURCES
    ${QHEXVIEW_SOURCES}
    ${REDASM_TEST_SOURCES}
    ${WIDGETS_SOURCES}
    ${DIALOGS_SOURCES}
    ${MODELS_SOURCES}
    ${RENDERER_SOURCES}
    ${UI_SOURCES}
    main.cpp
    mainwindow.cpp
    themeprovider.cpp
    redasmsettings.cpp
    disassembleractions.cpp)

set(FORMS
    ${WIDGETS_UIS}
    ${DIALOGS_UIS}
    ${UI_UIS}
    mainwindow.ui)

set(RESOURCES
    resources.qrc
    themes.qrc)

if(WIN32)
    SET(GUI_TYPE WIN32)
endif()

add_executable(${PROJECT_NAME} ${GUI_TYPE}
               ${SOURCES}
               ${HEADERS}
               ${FORMS}
               ${RESOURCES}
               ${CMAKE_SOURCE_DIR}/res/windows/resources.rc)

target_include_directories(${PROJECT_NAME} PUBLIC LibREDasm QHexEdit)
add_dependencies(${PROJECT_NAME} LibREDasm)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets LibREDasm)
else()
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets pthread LibREDasm)
endif()

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") AND DEBUG_STL_ITERATORS)
    target_compile_options(${PROJECT_NAME} PRIVATE -D_GLIBCXX_DEBUG)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-missing-braces)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    if(ENABLE_ADDRESS_SANITIZER)
        target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    elseif(ENABLE_THREAD_SANITIZER)
        target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    endif()
endif()

# Post build commands
set(LIBREDASM_BIN ${CMAKE_BINARY_DIR}/LibREDasm/LibREDasm${CMAKE_SHARED_LIBRARY_SUFFIX})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${LIBREDASM_BIN} ${CMAKE_BINARY_DIR})

