project(utils)


if(WIN32)
    add_library(utils src/utils_windows.cpp)
else()
    if(UNIX)
        if(APPLE)
            add_library(utils src/utils_darwin.cpp)
        else()
            add_library(utils src/utils_linux.cpp)
        endif()
    endif()
endif()

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)