FILE(GLOB FORM_DBO_H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
INSTALL(FILES ${FORM_DBO_H_FILES}
        DESTINATION include/Wt/Form/Dbo)
