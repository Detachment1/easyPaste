cmake_minimum_required(VERSION 3.13 FATAL_ERROR) # target_link_directories

project(tray)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Generate 'compile_commands.json' for clang_complete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SRCS
        ${CMAKE_CURRENT_LIST_DIR}/include/*.h
        ${CMAKE_CURRENT_LIST_DIR}/sources/*.ico
        ${CMAKE_CURRENT_LIST_DIR}/sources/*.png)

if(WIN32)
	list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/tray_windows.c)
else()
	if(UNIX)
		if(APPLE)
			find_library(COCOA Cocoa REQUIRED)
			list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/tray_darwin.m)
		else()
			FIND_PACKAGE(PkgConfig)
			PKG_CHECK_MODULES(APPINDICATOR REQUIRED appindicator3-0.1)
			list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/tray_linux.c)
		endif()
	endif()
endif()

add_library(tray STATIC ${SRCS})
set_property(TARGET tray PROPERTY C_STANDARD 99)
if(WIN32)
	target_compile_definitions(tray PRIVATE TRAY_WINAPI=1 WIN32_LEAN_AND_MEAN NOMINMAX)
	if(MSVC)
		target_compile_options(tray PRIVATE "/MT$<$<CONFIG:Debug>:d>")
	endif()
else()
        if(UNIX)
                if(APPLE)
					target_compile_definitions(tray PRIVATE TRAY_APPKIT=1)
					target_link_libraries(tray PRIVATE ${COCOA})
                else()
					target_compile_options(tray PRIVATE ${APPINDICATOR_CFLAGS})
					target_link_directories(tray PRIVATE ${APPINDICATOR_LIBRARY_DIRS})
					target_compile_definitions(tray PRIVATE TRAY_APPINDICATOR=1)
					target_link_libraries(tray PRIVATE ${APPINDICATOR_LIBRARIES})
                endif()
        endif()
endif()

add_library(tray::tray ALIAS tray)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

configure_file(${CMAKE_CURRENT_LIST_DIR}/sources/icon.ico ${CMAKE_BINARY_DIR}/sources/icon.ico COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/sources/icon.png ${CMAKE_BINARY_DIR}/sources/icon.png COPYONLY)

install (FILES ${CMAKE_BINARY_DIR}/sources/icon.ico
    DESTINATION sources)

install (FILES ${CMAKE_BINARY_DIR}/sources/icon.png
    DESTINATION sources)

#INSTALL(TARGETS tray tray DESTINATION lib)
#
#IF(NOT WIN32)
#	INSTALL(FILES tray.h DESTINATION include)
#ENDIF()

