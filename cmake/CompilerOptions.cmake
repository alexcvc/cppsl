# Compiler options with hardening flags

list(APPEND compiler_options
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:RELEASE>:-O2>
        $<$<CONFIG:DEBUG>:-O0 -g -p -pg>)

list(APPEND compiler_definitions
        $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:_FORTIFY_SOURCE=2>
        )

list(APPEND linker_flags
        $<$<NOT:$<CXX_COMPILER_ID:AppleClang>>:-Wl,-z,defs>
        $<$<NOT:$<CXX_COMPILER_ID:AppleClang>>:-Wl,-z,now>
        $<$<NOT:$<CXX_COMPILER_ID:AppleClang>>:-Wl,-z,relro>
        # Clang doesn't support these hardening flags
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-Wl,-pie>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-fpie>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-pipe>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-static-libstdc++>
        $<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>
        $<$<CONFIG:DEBUG>:-fsanitize=address>
        $<$<CONFIG:DEBUG>:-fsanitize=leak>
        $<$<CONFIG:DEBUG>:-fsanitize=undefined>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>>:-fstack-clash-protection>
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>>:-fbounds-check>
        -fstack-protector
        -fPIC)
