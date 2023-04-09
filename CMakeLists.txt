cmake_minimum_required(VERSION 3.13 FATAL_ERROR) # target_link_directories

project(easyPaste)

set(CMAKE_CXX_STANDARD 11)
# set a project version
set (easyPaste_VERSION_MAJOR 1)
set (easyPaste_VERSION_MINOR 1)
set (easyPaste_VERSION_PATCH 0)
set (easyPaste_VERSION "${easyPaste_VERSION_MAJOR}.${easyPaste_VERSION_MINOR}.${easyPaste_VERSION_PATCH}")

# Generate 'compile_commands.json' for clang_complete
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

############################################################
# Create a library
############################################################

add_subdirectory(utils)
add_subdirectory(tray)


############################################################
# Create an executable
############################################################

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        utils
        tray
        )

############################################################
# Install
############################################################

# Binaries
install (TARGETS easyPaste DESTINATION bin)

# Library
# Note: may not work on windows
install (TARGETS utils
    LIBRARY DESTINATION lib)

install (TARGETS tray
    LIBRARY DESTINATION lib)



############################################################
# Create tgz
############################################################

if( MINGW )
    message( STATUS "    Installing system-libraries: MinGW DLLs." )
    get_filename_component( Mingw_Path ${CMAKE_CXX_COMPILER} PATH )
    set( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${Mingw_Path}/libgcc_s_seh-1.dll ${Mingw_Path}/libwinpthread-1.dll ${Mingw_Path}/libstdc++-6.dll )
endif( MINGW )
include( InstallRequiredSystemLibraries )

# Actually install it when make install is called.
# Note, this works with CPack
if( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS )
    install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT System )
endif( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS )

set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_VERSION ${easyPaste_VERSION})

include(CPack)