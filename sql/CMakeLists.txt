set(MAIN_DB_FN ${CMAKE_BINARY_DIR}/params.db)

add_custom_target(params_sqlite ALL DEPENDS ${MAIN_DB_FN} SOURCES params.sql)

add_custom_command(
    OUTPUT ${MAIN_DB_FN}
    MAIN_DEPENDENCY params.sql
    COMMAND sqlite3 ${MAIN_DB_FN} < params.sql
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
