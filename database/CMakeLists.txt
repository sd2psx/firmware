find_package (Python COMPONENTS Interpreter)


set(GAMEDB_PS1_BIN "gamedbps1.dat")
set(GAMEDB_PS1_OBJ "${CMAKE_CURRENT_BINARY_DIR}/gamedbps1.o")

set(GAMEDB_PS2_BIN "gamedbps2.dat")
set(GAMEDB_PS2_OBJ "${CMAKE_CURRENT_BINARY_DIR}/gamedbps2.o")

add_custom_command(OUTPUT "${GAMEDB_PS1_OBJ}"
                    COMMAND ${Python_EXECUTABLE} ARGS parse_db.py ps1
                    COMMAND ${CMAKE_OBJCOPY} ARGS --input-target=binary --output-target=elf32-littlearm --binary-architecture arm --rename-section .data=.rodata ${GAMEDB_PS1_BIN} ${GAMEDB_PS1_OBJ}
                    COMMAND ${CMAKE_COMMAND} ARGS -E remove_directory ps1
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Generating game db binary for ps1")

#add_custom_command(OUTPUT "${GAMEDB_PS2_OBJ}"
#                    COMMAND ${Python_EXECUTABLE} ARGS parse_db.py ps2
#                    COMMAND ${CMAKE_OBJCOPY} ARGS --input-target=binary --output-target=elf32-littlearm --binary-architecture arm --rename-section .data=.rodata ${GAMEDB_PS2_BIN} ${GAMEDB_PS2_OBJ}
#                    COMMAND ${CMAKE_COMMAND} ARGS -E remove_directory ps2
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                    COMMENT "Generating game db binary for ps2")

add_custom_target(gamedbobjs DEPENDS 
                    "${GAMEDB_PS1_OBJ}" 
                    #"${GAMEDB_PS2_OBJ}"
                    )

add_library(gamedb INTERFACE)
add_dependencies(gamedb gamedbobjs)

target_link_libraries(gamedb INTERFACE 
                    ${GAMEDB_PS1_OBJ} 
                    #${GAMEDB_PS2_OBJ}
                    )

