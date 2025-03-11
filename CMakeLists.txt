cmake_minimum_required(VERSION 3.5)
project(RegisterExe LANGUAGES CXX RC)

option(REGISTER_EXE_CONSOLE "Build RegisterExe as a console application." ON)

# sources
set(sources
    main.cpp
    resources.rc
)

add_executable(RegisterExe ${sources})

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# static link
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    target_compile_options(RegisterExe
        PUBLIC
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
        $<$<CONFIG:MinSizeRel>:/MT>
    )
    target_link_options(RegisterExe PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
    
    # Unicode support
    target_compile_definitions(RegisterExe PRIVATE UNICODE _UNICODE)
    
    # Required libraries
    target_link_libraries(RegisterExe PRIVATE Shlwapi.lib)
else()
    # for mingw
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    
    # Required libraries
    target_link_libraries(RegisterExe 
        -static
        shlwapi
        winmm
    )
endif()

# Output directory
set_target_properties(RegisterExe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Admin privileges
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:EMBED /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
endif()