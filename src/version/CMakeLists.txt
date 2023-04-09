add_custom_target(version_generator ALL
                    COMMAND ${CMAKE_COMMAND} 
                                -D SCRIPT_TEMPLATE=${CMAKE_CURRENT_SOURCE_DIR}/template/version.c 
                                -D SCRIPT_WORKING_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                                -D SCRIPT_OUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/version.c
                                -P ${CMAKE_CURRENT_SOURCE_DIR}/script.cmake
                    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/version.c)