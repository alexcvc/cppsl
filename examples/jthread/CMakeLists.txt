set(EXE_TARGET_NAME jthread)

set(${EXE_TARGET_NAME}_SRC
        main.cpp
)

find_package(Threads REQUIRED)

add_executable(${EXE_TARGET_NAME} ${${EXE_TARGET_NAME}_SRC})

target_link_libraries(${EXE_TARGET_NAME} Threads::Threads)
target_link_libraries(${EXE_TARGET_NAME} cppsl::cppsl)

install(TARGETS ${EXE_TARGET_NAME} DESTINATION bin)
