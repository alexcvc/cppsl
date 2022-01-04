set(EXE_TARGET_NAME expapp_lib_header_only)

set(${EXE_TARGET_NAME}_SRC
        main.cpp
)

add_executable(${EXE_TARGET_NAME} ${${EXE_TARGET_NAME}_SRC})

target_link_libraries(${EXE_TARGET_NAME} cppsl::cppsl_header_only)

install(TARGETS ${EXE_TARGET_NAME} DESTINATION bin)
