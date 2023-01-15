
set(GAMEDB_PS1_BIN "gamedbps1.dat")
set(GAMEDB_PS1_OBJ "${CMAKE_CURRENT_BINARY_DIR}/gamedbps1.o")

add_custom_command(OUTPUT "${GAMEDB_PS1_OBJ}"
                    COMMAND ${CMAKE_OBJCOPY} ARGS --input-target=binary --output-target=elf32-littlearm --binary-architecture arm --rename-section .data=.rodata ${GAMEDB_PS1_BIN} ${GAMEDB_PS1_OBJ}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Generating game db binary for ps1")

add_custom_target(gamedbobjs DEPENDS "${GAMEDB_PS1_OBJ}")

add_library(gamedb INTERFACE)

target_link_libraries(gamedb INTERFACE ${GAMEDB_PS1_OBJ})
#set_property(TARGET gamedb PROPERTY IMPORTED_OBJECTS "${GAMEDB_PS1_OBJ}")

