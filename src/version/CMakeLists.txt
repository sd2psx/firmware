
find_package(Git QUIET)

if(Git_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} fetch --all --tags --force)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --exact-match HEAD --exclude=latest
                    OUTPUT_VARIABLE SD2PSX_VERSION)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    OUTPUT_VARIABLE SD2PSX_COMMIT)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                    OUTPUT_VARIABLE SD2PSX_BRANCH)
    string(REGEX REPLACE "\n$" "" SD2PSX_VERSION "${SD2PSX_VERSION}")
    string(REGEX REPLACE "\n$" "" SD2PSX_COMMIT "${SD2PSX_COMMIT}")
    string(REGEX REPLACE "\n$" "" SD2PSX_BRANCH "${SD2PSX_BRANCH}")
    if("${SD2PSX_VERSION}" STREQUAL "")
        set(SD2PSX_VERSION "${SD2PSX_BRANCH}#${SD2PSX_COMMIT}")
    endif()
else()
    set(SD2PSX_VERSION "None")
    set(SD2PSX_COMMIT "None")
    set(SD2PSX_BRANCH "None")
endif()
    

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/template/version.c ${CMAKE_CURRENT_BINARY_DIR}/version.c @ONLY)
